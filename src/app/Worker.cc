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
#include "app/messages/AvailabilityMessage_m.h"
#include "app/messages/AckMessage_m.h"
#include "app/messages/DataMessage_m.h"
#include "app/messages/ResponseMessage_m.h"
#include "app/messages/AckTimerMessage_m.h"
#include "app/messages/LoadBalanceTimerMessage_m.h"
#include "app/messages/ComputationTimerMessage_m.h"
#include "app/messages/UpdateAvailabilityMessage_m.h"

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

        // Initialize the probability to be still available after computation
        stillAvailableProbability = false;

        // Registering all signals
        stopHelp = registerSignal("stop_bus_help_rq");
        stopDataMessages = registerSignal("stop_sending_data");
        startResponseMessages = registerSignal("start_getting_response");
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
    // Your application has received a beacon message from another car or RSU
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
}

void Worker::onWSA(veins::DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from a bus
}

void Worker::handleSelfMsg(cMessage* msg)
{
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

    // Timer for re-send response message
    if (AckTimerMessage* ackTimerMessage = dynamic_cast<AckTimerMessage*>(msg)) {
        int hostIndex = ackTimerMessage->getHostIndex();
        double completionTime = ackTimerMessage->getTaskComputationTime();
        int taskID = ackTimerMessage->getTaskID();
        int partitionID = ackTimerMessage->getPartitionID();
        sendAgainResponse(hostIndex, completionTime, taskID, partitionID);
    }
}

void Worker::handlePositionUpdate(cObject* obj)
{
    // The vehicle has moved. Code that reacts to new positions goes here.
    // Member variables such as currentPosition and currentSpeed are updated in the parent class

    veins::DemoBaseApplLayer::handlePositionUpdate(obj);

    lastDroveAt = simTime();
}
