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

#include "TaskGenerator.h"

#include "app/messages/HelpMessage_m.h"
#include "app/messages/AvailabilityMessage_m.h"
#include "app/messages/AckMessage_m.h"
#include "app/messages/DataMessage_m.h"
#include "app/messages/ResponseMessage_m.h"
#include "app/messages/AckTimerMessage_m.h"
#include "app/messages/LoadBalanceTimerMessage_m.h"
#include "app/messages/ComputationTimerMessage_m.h"
#include "app/messages/UpdateAvailabilityMessage_m.h"
#include "app/vehiclesHandling/BusVehicleInfo.h"
#include <iostream>
#include "veins/modules/mobility/traci/TraCIConnection.h"
#include "veins/modules/mobility/traci/TraCIConstants.h"


using namespace task_offloading;

Define_Module(task_offloading::TaskGenerator);

void TaskGenerator::initialize(int stage)
{
    veins::DemoBaseApplLayer::initialize(stage);

    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        lastDroveAt = simTime();

        // BUS SECTION
        // Set the BUS index
        busIndex = findHost()->getIndex();

        // Initialize the BUS state
        busState = BusContext(new Help);

        // Initialize the load balancing algorithm
        loadBalancingAlgorithm = check_and_cast<BaseSorting*>(findModuleByPath("task_offloading.loadBalancingAlgorithm"));

        // Registering all signals
        startTask = registerSignal("task_started");
        stopTask = registerSignal("task_stopped");
        startBalance = registerSignal("start_balance_loading");
        stopBalance = registerSignal("stop_balance_loading");
        startHelp = registerSignal("start_bus_help_rq");
        startDataMessages = registerSignal("start_sending_data");
        stopResponseMessages = registerSignal("stop_getting_response");
    }
}

void TaskGenerator::finish()
{
    veins::DemoBaseApplLayer::finish();
}

void TaskGenerator::onBSM(veins::DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU
}

void TaskGenerator::onWSM(veins::BaseFrame1609_4* wsm)
{
    /************************************************************************
      Your application has received a data message from another car or RSU
    ************************************************************************/

    // SECTION - When the bus receives the ok messages
    if (AvailabilityMessage* availabilityMessage = dynamic_cast<AvailabilityMessage*>(wsm)) {
        handleAvailabilityMessage(availabilityMessage);
    }

    // SECTION - When the bus receive the response message
    if (ResponseMessage* responseMessage = dynamic_cast<ResponseMessage*>(wsm)) {
        handleResponseMessage(responseMessage);
    }
}

void TaskGenerator::onWSA(veins::DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
}

void TaskGenerator::handleSelfMsg(cMessage* msg)
{
    // This method is for self messages (mostly timers)
    // Timer for help message
    if (LoadBalanceTimerMessage* loadBalanceMsg = dynamic_cast<LoadBalanceTimerMessage*>(msg)) {
        // If I'm the bus and I've received help then load balance
        // otherwise back to help messages
        if (findHost()->getIndex() == busIndex && helpers.size() > 0 && busState.getCurrentState() == 1) {
            // Call to load balancing function
            balanceLoad();
        } else if (helpers.size() == 0) {
            // Disable load balancing
            busState.setState(new Help);
        }
    }

    // Timer for data computation
    if (ComputationTimerMessage* computationTimerMessage = dynamic_cast<ComputationTimerMessage*>(msg)) {
        const DataMessage* data = computationTimerMessage->getData();
        sendAgainData(data);
    }

    // Timer for data message
    if (DataMessage* dataMsg = dynamic_cast<DataMessage*>(msg)) {
        // Send signal for data message statistic with the host ID
        emit(startDataMessages, dataMsg->getHostIndex());

        sendDown(dataMsg->dup());
    }

    // Timer for ACK message
    if (AckMessage* ackMsg = dynamic_cast<AckMessage*>(msg)) {
        sendDown(ackMsg->dup());
    }
}

void TaskGenerator::handlePositionUpdate(cObject* obj)
{
    // The vehicle has moved. Code that reacts to new positions goes here.
    // Member variables such as currentPosition and currentSpeed are updated in the parent class

    veins::DemoBaseApplLayer::handlePositionUpdate(obj);

    vehicleHandler();

    lastDroveAt = simTime();
}
