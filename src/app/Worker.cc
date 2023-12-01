//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "Worker.h"

#include "app/messages/HelpMessage_m.h"
#include "app/messages/BeaconMessage_m.h"
#include "app/messages/AvailabilityMessage_m.h"
#include "app/messages/AckMessage_m.h"
#include "app/messages/DataMessage_m.h"
#include "app/messages/ResponseMessage_m.h"
#include "app/messages/AckTimerMessage_m.h"
#include "app/messages/LoadBalanceTimerMessage_m.h"
#include "app/messages/ComputationTimerMessage_m.h"
#include "app/messages/UpdateAvailabilityMessage_m.h"
#include "app/messages/TotalComputationTimerMessage_m.h"

using namespace task_offloading;
using namespace inet;

Define_Module(task_offloading::Worker);

void Worker::initialize(int stage)
{
    veins::VeinsInetApplicationBase::initialize(stage);

    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        lastDroveAt = simTime();

        // Initialize my cpu freq
        cpuFreq = 0;

        // Initialize data partition id
        currentDataPartitionId = -1;

        //common vehicle load
        availableLoad = par("commonVehicleLoad").doubleValue();

        // Initialize the probability to be still available after computation
        stillAvailableProbability = false;

        if(par("retryFactorTime").doubleValue()<1) {
            throw cRuntimeError("retryFactorTime cannot be lower than 1");
        }

        // Registering all signals
        stopHelp = registerSignal("stop_bus_help_rq");
        stopDataMessages = registerSignal("stop_sending_data");
        startResponseMessages = registerSignal("start_getting_response");
        stopBeaconMessages = registerSignal("stopBeaconMessages");
        availableMessageSent = registerSignal("available_message_sent");
        availableMessageLoad = registerSignal("available_message_load");
    }
}

void Worker::handleStartOperation(inet::LifecycleOperation* doneCallback)
{
    // Starting operation for worker app
    veins::VeinsInetApplicationBase::handleStartOperation(doneCallback);
}

void Worker::handleStopOperation(inet::LifecycleOperation* doneCallback)
{
    veins::VeinsInetApplicationBase::handleStopOperation(doneCallback);
    // Close the socket
    socket.close();
}

void Worker::finish()
{
    veins::VeinsInetApplicationBase::finish();
}

void Worker::processPacket(std::shared_ptr<inet::Packet> pk)
{
    /************************************************************************
      Your application has received a data message from a task generator
    ************************************************************************/
    try {
        if (pk->hasData<BasePacket>()) {
            auto packet = pk->peekData<BasePacket>();
            BasePacket* data = packet->dup();

            // SECTION - When the host receive an help message
            if (data->getType() == HELP) {
                auto dataFromPacket = pk->peekData<HelpMessage>();
                HelpMessage* helpMessage = dataFromPacket->dup();
                handleHelpMessage(helpMessage);
            }

            // SECTION - When the host receive the data message
            if (data->getType() == DATA) {
                auto dataFromPacket = pk->peekData<DataMessage>();
                DataMessage* dataMessage = dataFromPacket->dup();
                handleDataMessage(dataMessage);
            }

            // SECTION - When the host receive the ACK message
            if (data->getType() == ACK) {
                currentDataPartitionId = -1;

                // Color the vehicle in white when computation ends
                getParentModule()->getDisplayString().setTagArg("i", 1, "white");
            }

            if (data->getType() == BEACON) {
                emit(stopBeaconMessages, simTime());
            }
        }
    } catch (cException e) {
        EV << "Package type not expected\nError: " << e.getFormattedMessage() << std::endl;
    }
}

void Worker::setTaskAvailabilityTimer(int taskId, int taskSize){
    //Set task availability timer
    // double bitRate = getModuleByPath(".^.nic.mac1609_4")->par("bitrate").intValue() / 8.0;
    // FIXME -> Get the correct value for bitrate -> now it is 6Mbps
    double bitRate = 93750;
    double taskTransmissionTime = ceil(taskSize/bitRate);
    double taskTimer = (1 + taskTransmissionTime*1.1)*par("retryFactorTime").doubleValue();

    auto callback = [this]() {
        //Color the vehicle in white when task availability timer runs out
        getParentModule()->getDisplayString().setTagArg("i", 1, "white");
        //Reset common vehicle load
        availableLoad = par("commonVehicleLoad").doubleValue();  //Only with one task
    };

    double time = taskTimer;

    TotalComputationTimerMessage* tcm = new TotalComputationTimerMessage("taskAvailabilityTimerMessage");
    tcm->setTaskId(taskId);
    taskAvailabilityTimers.insert(std::pair<int,TotalComputationTimerMessage*>(taskId,tcm));

    // Start the timer
    timerManager.create(veins::TimerSpecification(callback).oneshotIn(time));
}

void Worker::resetTaskAvailabilityTimer(int taskId) {
    //stop task availability timer
//    if(taskAvailabilityTimers.find(taskId)!=taskAvailabilityTimers.end()){
        cancelAndDelete(taskAvailabilityTimers.at(taskId));
        taskAvailabilityTimers.erase(taskId);
//    }
}

bool Worker::isNewPartition(DataMessage* dataMessage){
    auto key = std::pair<int,int>(dataMessage->getTaskId(),dataMessage->getPartitionId());
    return responseCache.find(key) == responseCache.end();
}

void Worker::handleHelpMessage(HelpMessage* helpMessage)
{
    // I've received an help request from bus
    // Emit the help message received
    emit(stopHelp, simTime());

    // First check if I met requirements for the bus
    double minimumLoadRequested = helpMessage->getMinimumLoadRequested();

    // Check my current load
    double currentVehicleLoad = par("randomVehicleFreeLoadPercentage").doubleValue() * par("commonVehicleLoad").doubleValue();
    availableLoad = availableLoad - currentVehicleLoad;

    // Emit the signal for my current load
    emit(availableMessageLoad, currentVehicleLoad);

    // Check my current CPU freq
    double CPUFreq = par("randomVehicleCpuFreq").doubleValue();

    // Set my CPU freq
    cpuFreq = CPUFreq;

    // If I met requirements send an available message
    if (currentVehicleLoad >= minimumLoadRequested) {
        //start task availability timer
        setTaskAvailabilityTimer(helpMessage->getId(),helpMessage->getTaskSize());

        // Color the vehicle icon in blue
        getParentModule()->getDisplayString().setTagArg("i", 1, "blue");

        // Prepare the availability message
        auto available = makeShared<AvailabilityMessage>();

        // Populate the message
        available->setHostID(getParentModule()->getIndex());
        available->setIndex(getParentModule()->getName());
        available->setAvailableLoad(currentVehicleLoad);
        available->setCpuFreq(cpuFreq);
        available->setVehicleAngle(traciVehicle->getAngle());
        available->setVehicleSpeed(traciVehicle->getSpeed());
        // FIXME -> get the correct module for mobility
        // veins::TraCIMobility* mobilityMod = check_and_cast<veins::TraCIMobility*>(findModuleByPath("^.veinsmobility"));
        double cx = mobility->getCurrentPosition().x;
        double cy = mobility->getCurrentPosition().y;
        available->setVehiclePositionX(cx);
        available->setVehiclePositionY(cy);
        available->setChunkLength(B(500));

        // Schedule the ok message
        double time = par("vehicleAvailabilityMessageTime").doubleValue();
        timerManager.create(veins::TimerSpecification(simulateAvailabilityTime(available->dup())).oneshotIn(time));
    }
}

void Worker::handleDataMessage(DataMessage* dataMessage)
{
    // Emit the signal for have received data message
    emit(stopDataMessages, dataMessage->getHostIndex());

    // Calculate time for computation
    double CPI = dataMessage->getCpi();
    double I = dataMessage->getLoadToProcess();
    double CR = cpuFreq;

    double timeToCompute = CPI * I * (1 / CR);
    EV<<"TIME TO COMPUTE"<<timeToCompute<<endl;

    auto key = std::pair<int,int>(dataMessage->getTaskId(),dataMessage->getPartitionId());

    //if the cache is not empty it resends the response message tied to this data message
    if(!isNewPartition(dataMessage)){
        sendAgainResponse(responseCache.at(key));
        return;
    }

    //reset the task availability timer
    resetTaskAvailabilityTimer(dataMessage->getTaskId());

    // Color the vehicle in red when computing
    getParentModule()->getDisplayString().setTagArg("i", 1, "red");

    // Update the partition ID
    currentDataPartitionId = dataMessage->getPartitionId();

    // Update if I'll be still available
    stillAvailableProbability = par("stillAvailableProbability").doubleValue() > par("stillAvailableThreshold").doubleValue();
    if(stillAvailableProbability) {
        setTaskAvailabilityTimer(dataMessage->getTaskId(), dataMessage->getTaskSize());
    }


    // Prepare the response message
    auto responseMessage = makeShared<ResponseMessage>();

    // Populate the response message
    responseMessage->setHostIndex(dataMessage->getHostIndex());
    responseMessage->setStillAvailable(stillAvailableProbability);
    responseMessage->setDataComputed(dataMessage->getLoadToProcess());
    responseMessage->setTimeToCompute(timeToCompute);
    responseMessage->setTaskID(dataMessage->getTaskId());
    responseMessage->setPartitionID(dataMessage->getPartitionId());
    responseMessage->setChunkLength(B(dataMessage->getLoadToProcess()));
    // L3Address worker = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
    // responseMessage->setSenderAddress(worker);

    //Insert response message in response cache
    responseCache.insert(std::pair<std::pair<int, int>, ResponseMessage*>(key, responseMessage->dup()));

    // Schedule the response message
    // Create the computation timer to simulate the computation time
    double time = timeToCompute;

    // Schedule the timer
    timerManager.create(veins::TimerSpecification(simulateResponseTime(responseMessage->dup())).oneshotIn(time));

    // Generate ACK timer if parameter useAcks is false
    // to achieve secure protocol manually and if I'm not still available
    if (!(par("useAcks").boolValue()) && !(stillAvailableProbability)) {
        //Calculate bitrate conversion from megabit to megabyte
        // double bitRate = getModuleByPath(".^.nic.mac1609_4")->par("bitrate").intValue() / 8.0;
        // FIXME -> Get the correct value for bitrate -> now it is 6Mbps
        double bitRate = 93750;
        double transferTime = dataMessage->getLoadToProcess()/bitRate;

        time = (timeToCompute + transferTime + par("ackMessageThreshold").doubleValue());
        timerManager.create(veins::TimerSpecification(sendAgainResponse(responseMessage->dup())).oneshotIn(time));
    }
}

std::function<void()> Worker::sendAgainResponse(ResponseMessage* response)
{
    if (currentDataPartitionId == response->getPartitionID()) {
        auto newResponse = makeShared<ResponseMessage>();

        // Fill fields of response message with previous response message

        /**************************************************************************
         * This has to be done because in veins if you send a message duplicate   *
         * it will be discarded from MAC L2 because it's a message that the       *
         * single node "read" as already received                                 *
         *************************************************************************/

        newResponse->setHostIndex(response->getHostIndex());
        newResponse->setStillAvailable(response->getStillAvailable());
        newResponse->setDataComputed(response->getDataComputed());
        newResponse->setTimeToCompute(response->getTimeToCompute());
        newResponse->setTaskID(response->getTaskID());
        newResponse->setPartitionID(response->getPartitionID());
        newResponse->setChunkLength(B(response->getChunkLength()));
        // L3Address worker = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
        // newResponse->setSenderAddress(worker);

        // Create the computation timer to simulate the computation time
        double time = response->getTimeToCompute();

        // Schedule the new duplicate response message
        timerManager.create(veins::TimerSpecification(simulateResponseTime(newResponse->dup())).oneshotIn(time));

        // Restart the ACK timer
        double transferTime = 10.0;

        time = (transferTime + response->getTimeToCompute() + par("ackMessageThreshold").doubleValue());

        timerManager.create(veins::TimerSpecification(sendAgainResponse(newResponse->dup())).oneshotIn(time));
    }

    return 0;
}

std::function<void()> Worker::simulateAvailabilityTime(AvailabilityMessage* availabilityMessage)
{
    // Emit the signal of ok message sent
    emit(availableMessageSent, simTime());

    // Send the ok message
    auto availability = availabilityMessage->dupShared();

    auto availabilityPkt = createPacket("availability_duplicate");
    availabilityPkt->insertAtBack(availability);
    sendPacket(std::move(availabilityPkt));

    return 0;
}

std::function<void()> Worker::simulateResponseTime(ResponseMessage* responseMessage) {
    // Send signal for response message statistic with the host ID
    emit(startResponseMessages, responseMessage->getHostIndex());
    if(responseMessage->getStillAvailable()) {
        getParentModule()->getDisplayString().setTagArg("i", 1, "blue");
    } else{
        // Color the vehicle in white when send down the response
        getParentModule()->getDisplayString().setTagArg("i", 1, "white");
        //Reset common vehicle load
        availableLoad = par("commonVehicleLoad").doubleValue();  //Only with one task
    }

    // Send the response message
    auto response = responseMessage->dupShared();

    auto responsePkt = createPacket("response_duplicate");
    responsePkt->insertAtBack(response);
    sendPacket(std::move(responsePkt));

    return 0;
}
