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

        // Initialize the index of the generator
        generatorIndex = 0;

        // Initialize the total data partitions I've received
        dataPartitionsReceived = 0;

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

                EV << "Message received with partition " << dataMessage->getPartitionId() << std::endl;

                // Check if the data message is for me
                if (dataMessage->getHostIndex() == getParentModule()->getIndex()) {
                    handleDataMessage(dataMessage);
                }
            }

            // SECTION - When the host receive the ACK message
            if (data->getType() == ACK) {
                auto dataFromPacket = pk->peekData<AckMessage>();
                AckMessage* ackMessage = dataFromPacket->dup();

                // Check if the ack message is for me
                if (ackMessage->getHostIndex() == getParentModule()->getIndex()) {
                    // Cancel the timer
                    veins::TimerManager::TimerHandle timer = getTimer(ackMessage->getPartitionID());
                    timerManager.cancel(timer);

                    // Remove the entry from the cache
                    auto key = std::pair<int,int>(ackMessage->getTaskID(), ackMessage->getPartitionID());
                    responseCache.erase(key);

                    // Color the vehicle in white when computation ends
                    getParentModule()->getDisplayString().setTagArg("i", 1, "white");
                }
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
    // Set task availability timer
    double bitRate = findModuleByPath(".^.wlan[*]")->par("bitrate").doubleValue() / 8.0;
    double taskTransmissionTime = ceil(taskSize/bitRate);
    double taskTimer = (1 + taskTransmissionTime*1.1)*par("retryFactorTime").doubleValue();

    auto callback = [this]() {
        // Color the vehicle in white when task availability timer runs out
        getParentModule()->getDisplayString().setTagArg("i", 1, "white");
        // Reset common vehicle load
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
    // Stop task availability timer
    if(taskAvailabilityTimers.find(taskId) != taskAvailabilityTimers.end()){
        cancelAndDelete(taskAvailabilityTimers.at(taskId));
        taskAvailabilityTimers.erase(taskId);
    }
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
        // Start task availability timer
        setTaskAvailabilityTimer(helpMessage->getId(),helpMessage->getTaskSize());

        // Color the vehicle icon in blue
        getParentModule()->getDisplayString().setTagArg("i", 1, "blue");

        // Prepare the availability message
        auto available = makeShared<AvailabilityMessage>();

        // Save the ID of the host that generates the task
        generatorIndex = helpMessage->getGeneratorIndex();

        // Populate the message
        available->setHostID(getParentModule()->getIndex());
        available->setIndex(getParentModule()->getName());
        available->setAvailableLoad(currentVehicleLoad);
        available->setCpuFreq(cpuFreq);
        available->setVehicleAngle(traciVehicle->getAngle());
        available->setVehicleSpeed(traciVehicle->getSpeed());
        double cx = mobility->getCurrentPosition().x;
        double cy = mobility->getCurrentPosition().y;
        available->setVehiclePositionX(cx);
        available->setVehiclePositionY(cy);
        available->setChunkLength(B(500));

        // Schedule the ok message
        double time = par("vehicleAvailabilityMessageTime").doubleValue();

        // The & inside the square brackets tells to capture all local variable
        // by value
        auto callback = [=]() {
            simulateAvailabilityTime(available->dup());
        };
        timerManager.create(veins::TimerSpecification(callback).oneshotIn(time));
    }
}

void Worker::handleDataMessage(DataMessage* dataMessage)
{
    // Emit the signal for have received data message
    emit(stopDataMessages, dataMessage->getHostIndex());

    // Increment the number of data partitions I've received
    dataPartitionsReceived++;

    // Calculate time for computation
    double CPI = dataMessage->getCpi();
    double I = dataMessage->getLoadToProcess();
    double CR = cpuFreq;

    double timeToCompute = CPI * I * (1 / CR);
    EV << "TIME TO COMPUTE" << timeToCompute << endl;

    auto key = std::pair<int,int>(dataMessage->getTaskId(), dataMessage->getPartitionId());

    // If the cache is not empty it resends the response message tied to this data message
    if(!(responseCache.find(key) == responseCache.end())){
        sendAgainResponse(responseCache.at(key));
        return;
    }

    // Reset the task availability timer
    resetTaskAvailabilityTimer(dataMessage->getTaskId());

    // Color the vehicle in red when computing
    getParentModule()->getDisplayString().setTagArg("i", 1, "red");

    // Update the partition ID
    currentDataPartitionId = dataMessage->getPartitionId();

    // Update if I'll be still available and this is the last data message
    stillAvailableProbability = par("stillAvailableProbability").doubleValue() > par("stillAvailableThreshold").doubleValue();
    if (stillAvailableProbability) {
        setTaskAvailabilityTimer(dataMessage->getTaskId(), dataMessage->getTaskSize());
    }

    // Prepare the response message
    auto responseMessage = makeShared<ResponseMessage>();

    // Populate the response message
    responseMessage->setHostIndex(getParentModule()->getIndex());
    responseMessage->setGeneratorIndex(generatorIndex);

    // If this is the last data partition then set the probability I'll
    // be still available, otherwise set it to true to prevent the vehicle
    // deletion by the TaskGenerator side
    if (dataPartitionsReceived == dataMessage->getResponsesExpected()) {
        responseMessage->setStillAvailable(stillAvailableProbability);
    } else {
        responseMessage->setStillAvailable(true);
    }

    responseMessage->setDataComputed(dataMessage->getLoadToProcess());
    responseMessage->setTimeToCompute(timeToCompute);
    responseMessage->setTaskID(dataMessage->getTaskId());
    responseMessage->setPartitionID(dataMessage->getPartitionId());
    responseMessage->setChunkLength(B(dataMessage->getLoadToProcess()));
    // L3Address worker = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
    // responseMessage->setSenderAddress(worker);

    // Insert response message in response cache
    responseCache.insert(std::pair<std::pair<int, int>, ResponseMessage*>(key, responseMessage->dup()));

    // Schedule the response message
    // Create the computation timer to simulate the computation time
    double time = timeToCompute;

    // Schedule the timer
    // The & inside the square brackets tells to capture all local variable
    // by value
    auto callback = [=]() {
        simulateResponseTime(responseMessage->dup());
    };
    timerManager.create(veins::TimerSpecification(callback).oneshotIn(time));

    // Generate ACK timer if parameter useAcks is false
    // to achieve secure protocol manually and if I'm not still available
    if (!(par("useAcks").boolValue()) && !(stillAvailableProbability)) {
        // Calculate bitrate conversion from megabit to megabyte
        double bitRate = findModuleByPath(".^.wlan[*]")->par("bitrate").doubleValue() / 8.0;;
        double transferTime = dataMessage->getLoadToProcess() / bitRate;

        time = (timeToCompute + transferTime + par("ackMessageThreshold").doubleValue());

        // The & inside the square brackets tells to capture all local variable
        // by value
        auto sendAgainCallback = [=]() {
            sendAgainResponse(responseMessage->dup());
        };
        // Save the timer in the timers map
        veins::TimerManager::TimerHandle timer = timerManager.create(veins::TimerSpecification(sendAgainCallback).oneshotIn(time));
        addTimer(currentDataPartitionId, timer);
    }
}

void Worker::sendAgainResponse(ResponseMessage* response)
{
    auto newResponse = makeShared<ResponseMessage>();

    // Fill fields of response message with previous response message

    /**************************************************************************
     * This has to be done because in veins if you send a message duplicate   *
     * it will be discarded from MAC L2 because it's a message that the       *
     * single node "read" as already received                                 *
     *************************************************************************/

    newResponse->setHostIndex(response->getHostIndex());
    newResponse->setGeneratorIndex(response->getGeneratorIndex());
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
    // The & inside the square brackets tells to capture all local variable
    // by value
    auto callback = [=]() {
        simulateResponseTime(newResponse->dup());
    };
    timerManager.create(veins::TimerSpecification(callback).oneshotIn(time));

    // Restart the ACK timer
    double transferTime = 10.0;

    time = (transferTime + response->getTimeToCompute() + par("ackMessageThreshold").doubleValue());

    // The & inside the square brackets tells to capture all local variable
    // by value
    auto sendAgainCallBack = [=]() {
        sendAgainResponse(newResponse->dup());
    };
    // Rewrite the value of the timer in timers map
    veins::TimerManager::TimerHandle timer = timerManager.create(veins::TimerSpecification(sendAgainCallBack).oneshotIn(time));
    addTimer(response->getPartitionID(), timer);
}

void Worker::simulateAvailabilityTime(AvailabilityMessage* availabilityMessage)
{
    // Emit the signal of ok message sent
    emit(availableMessageSent, simTime());

    // Send the ok message
    auto availability = availabilityMessage->dupShared();

    auto availabilityPkt = createPacket("availability_message");
    availabilityPkt->insertAtBack(availability);
    sendPacket(std::move(availabilityPkt));
}

void Worker::simulateResponseTime(ResponseMessage* responseMessage) {
    // Send signal for response message statistic with the host ID
    emit(startResponseMessages, responseMessage->getHostIndex());
    if(responseMessage->getStillAvailable()) {
        getParentModule()->getDisplayString().setTagArg("i", 1, "blue");
    } else {
        // Color the vehicle in white when send down the response
        getParentModule()->getDisplayString().setTagArg("i", 1, "white");
        // Reset common vehicle load
        availableLoad = par("commonVehicleLoad").doubleValue();  //Only with one task
    }

    // Send the response message
    auto response = responseMessage->dupShared();

    auto responsePkt = createPacket("response_message");
    responsePkt->insertAtBack(response);
    sendPacket(std::move(responsePkt));
}

void Worker::addTimer(int partitionID, veins::TimerManager::TimerHandle timer) {
    this->timers[partitionID] = timer;
}

veins::TimerManager::TimerHandle Worker::getTimer(int partitionID) {
    auto found = this->timers.find(partitionID);

    if (found != this->timers.end()) {
        return this->timers[partitionID];
    } else {
        return -1;
    }
}
