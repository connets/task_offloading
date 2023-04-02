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

#include "app/VeinsApp.h"

#include "app/messages/HelpMessage_m.h"
#include "app/messages/OkMessage_m.h"
#include "app/messages/AckMessage_m.h"
#include "app/messages/DataMessage_m.h"
#include "app/messages/ResponseMessage_m.h"
#include "app/messages/AckTimerMessage_m.h"
#include "app/messages/LoadBalanceTimerMessage_m.h"
#include "app/messages/ComputationTimerMessage_m.h"
#include "app/messages/UpdateAvailabilityMessage_m.h"

using namespace task_offloading;

Define_Module(task_offloading::VeinsApp);

void VeinsApp::initialize(int stage)
{
    veins::DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        lastDroveAt = simTime();
        sentHelpMessage = false;
        helpReceived = false;
        newRandomTime = 0;
        ackReceived = false;
        hostCpuFreq = 0;

        // BUS SECTION
        // Calculate BUS load
        double busLoad = par("randomVehicleLoadActual").doubleValue() * par("busVehicleLoad").doubleValue();
        double busFreq = par("randomCpuVehicleFreq").doubleValue();
        busIndex = 0;
        helpers[0] = HelperVehicleInfo(busLoad, busFreq, simTime(), busIndex);

        // Initialize the BUS state
        loadBalancingState = LoadBalancingContext(new Disabled);

        // Initialize the load balancing algorithm
        loadBalancingFIFO = dynamic_cast<FIFO*>(getParentModule()->getSubmodule("loadBalancingAlgorithm"));
        loadBalancingCompTime = dynamic_cast<ComputationTime*>(getParentModule()->getSubmodule("loadBalancingAlgorithm"));

        // Registering all signals
        startTask = registerSignal("task_started");
        stopTask = registerSignal("task_stopped");
        startBalance = registerSignal("start_balance_loading");
        stopBalance = registerSignal("stop_balance_loading");
        startHelp = registerSignal("start_bus_help_rq");
        stopHelp = registerSignal("stop_bus_help_rq");
        startDataMessages = registerSignal("start_sending_data");
        stopDataMessages = registerSignal("stop_sending_data");
        startResponseMessages = registerSignal("start_getting_response");
        stopResponseMessages = registerSignal("stop_getting_response");
        okMessageSent = registerSignal("ok_message_sent");
        okMessageLoad = registerSignal("ok_message_load");
    }
}

void VeinsApp::finish()
{
    veins::DemoBaseApplLayer::finish();
}

void VeinsApp::onBSM(veins::DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU
}

void VeinsApp::onWSM(veins::BaseFrame1609_4* wsm)
{
    /************************************************************************
      Your application has received a data message from another car or RSU
    ************************************************************************/

    // SECTION - When the host receive an help message
    if (HelpMessage* helpMsg = dynamic_cast<HelpMessage*>(wsm)) {
        busIndex = helpMsg->getVehicleIndex();
        handleHelpMessage(helpMsg);
    }

    // SECTION - When the bus receives the ok messages
    if (OkMessage* okMsg = dynamic_cast<OkMessage*>(wsm)) {
        handleOkMessage(okMsg);
    }

    // SECTION - When the host receive the data message
    if (DataMessage* dataMsg = dynamic_cast<DataMessage*>(wsm)) {
        handleDataMessage(dataMsg);
    }

    // SECTION - When the bus receive the response message
    if (ResponseMessage* responseMsg = dynamic_cast<ResponseMessage*>(wsm)) {
        handleResponseMessage(responseMsg);
    }

    // SECTION - When the host receive the ACK message
    if (AckMessage* ackMessage = dynamic_cast<AckMessage*>(wsm)) {
        if (findHost()->getIndex() == ackMessage->getHostIndex()) {
            ackReceived = true;
        }
    }

}

void VeinsApp::onWSA(veins::DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
}

void VeinsApp::handleSelfMsg(cMessage* msg)
{
    // This method is for self messages (mostly timers)
    // Timer for help message
    if (LoadBalanceTimerMessage* loadBalanceMsg = dynamic_cast<LoadBalanceTimerMessage*>(msg)) {
        // Set the load balance mode to active
        loadBalancingState.setState(new Active);

        // Effective balance of the load
        balanceLoad(loadBalanceMsg->getSimulationTime());
    }

    // Timer for re-send ACK messages
    if (AckTimerMessage* ackTimerMsg = dynamic_cast<AckTimerMessage*>(msg)) {
        sendAgainResponse(ackTimerMsg->getHostIndex(), ackTimerMsg->getTaskComputationTime());
    }

    // Timer for data computation
    if (ComputationTimerMessage* computationTimerMsg = dynamic_cast<ComputationTimerMessage*>(msg)) {
        sendAgainData(computationTimerMsg->getIndexHost(), computationTimerMsg->getLoadHost(), computationTimerMsg->getTaskComputationTime());
    }

    // Timer for ok message
    if (OkMessage* okMsg = dynamic_cast<OkMessage*>(msg)) {
        if (loadBalancingState.getCurrentState() == true) {
            // Color the available host in blue
            findHost()->getDisplayString().setTagArg("i", 1, "blue");

            // Send the ok message
            sendDown(okMsg->dup());

            // Emit the signal of ok message sent
            emit(okMessageSent, simTime());
        }
    }

    // Timer for data message
    if (DataMessage* dataMsg = dynamic_cast<DataMessage*>(msg)) {
        sendDown(dataMsg->dup());

        // Send signal for data message statistic with the host ID
        emit(startDataMessages, dataMsg->getHostIndex());
    }

    // Timer for response message
    if (ResponseMessage* responseMsg = dynamic_cast<ResponseMessage*>(msg)) {
        findHost()->getDisplayString().setTagArg("i", 1, "white");
        sendDown(responseMsg->dup());

        // Send signal for response message statistic with the host ID
        emit(startResponseMessages, responseMsg->getHostIndex());
    }

    // Timer for ACK message
    if (AckMessage* ackMsg = dynamic_cast<AckMessage*>(msg)) {
        sendDown(ackMsg->dup());
    }
}

void VeinsApp::handlePositionUpdate(cObject* obj)
{
    // The vehicle has moved. Code that reacts to new positions goes here.
    // Member variables such as currentPosition and currentSpeed are updated in the parent class

    veins::DemoBaseApplLayer::handlePositionUpdate(obj);

    vehicleHandler();

    lastDroveAt = simTime();
}
