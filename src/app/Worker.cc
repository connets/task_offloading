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

Define_Module(task_offloading::Worker);

void Worker::initialize(int stage)
{
    veins::DemoBaseApplLayer::initialize(stage);

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

void Worker::finish()
{
    veins::DemoBaseApplLayer::finish();
}

void Worker::onBSM(veins::DemoSafetyMessage* bsm)
{

}

void Worker::onWSM(veins::BaseFrame1609_4* wsm)
{
    /************************************************************************
      Your application has received a data message from a bus
    ************************************************************************/

    // SECTION - When the host receive an help message
    if (HelpMessage* helpMessage = dynamic_cast<HelpMessage*>(wsm)) {
        handleHelpMessage(helpMessage);
    }

    // SECTION - When the host receive the data message
    if (DataMessage* dataMessage = dynamic_cast<DataMessage*>(wsm)) {
        handleDataMessage(dataMessage);
    }

    // SECTION - When the host receive the ACK message
    if (AckMessage* ackMessage = dynamic_cast<AckMessage*>(wsm)) {
        // Check if I'm the host for the ack message
        if (ackMessage->getHostIndex() == findHost()->getIndex()) {
            currentDataPartitionId = -1;

            // Color the vehicle in white when computation ends
            findHost()->getDisplayString().setTagArg("i", 1, "white");
        }
    }

    if(BeaconMessage* bmsg = dynamic_cast<BeaconMessage*>(wsm)) {
            emit(stopBeaconMessages, simTime());
        }
}

void Worker::onWSA(veins::DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from a bus
}

void Worker::handleSelfMsg(cMessage* msg)
{
    //TODO timer based on cMessage and not on veins

    // Timer for ok message
    if (AvailabilityMessage* availabilityMessage = dynamic_cast<AvailabilityMessage*>(msg)) {
        // Emit the signal of ok message sent
        emit(availableMessageSent, simTime());

        // Send the ok message
        sendDown(availabilityMessage->dup());
    }

    // Timer for response message
    if (ResponseMessage* responseMessage = dynamic_cast<ResponseMessage*>(msg)) {
        // Send signal for response message statistic with the host ID
        emit(startResponseMessages, responseMessage->getHostIndex());

        // Color the vehicle in white when send down the response
        findHost()->getDisplayString().setTagArg("i", 1, "white");

        // Send the response message
        sendDown(responseMessage->dup());
    }

    //Timer for task
    if(TotalComputationTimerMessage* tcm = dynamic_cast<TotalComputationTimerMessage*>(msg)){
        //Color the vehicle in white when task availability timer runs out
        findHost()->getDisplayString().setTagArg("i", 1, "white");
        //Reset common vehicle load
        availableLoad = par("commonVehicleLoad").doubleValue();  //Only with one task
    }

}

void Worker::handlePositionUpdate(cObject* obj)
{
    // The vehicle has moved. Code that reacts to new positions goes here.
    // Member variables such as currentPosition and currentSpeed are updated in the parent class

    veins::DemoBaseApplLayer::handlePositionUpdate(obj);

    lastDroveAt = simTime();
}

void Worker::setTaskAvailabilityTimer(int taskId, int taskSize){
    //Set task availability timer
    double bitRate = getModuleByPath(".^.nic.mac1609_4")->par("bitrate").intValue() / 8.0;
    double taskTransmissionTime = ceil(taskSize/bitRate);
    double taskTimer = (1 + taskTransmissionTime*1.1)*par("retryFactorTime").doubleValue();

    TotalComputationTimerMessage* tcm = new TotalComputationTimerMessage("taskAvailabilityTimerMessage");
    tcm->setTaskId(taskId);
    taskAvailabilityTimers.insert(std::pair<int,TotalComputationTimerMessage*>(taskId,tcm));
    scheduleAfter(taskTimer, tcm);
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
