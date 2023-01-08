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

using namespace tirocinio;

Define_Module(tirocinio::VeinsApp);

void VeinsApp::initialize(int stage)
{
    veins::DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        lastDroveAt = simTime();
        sentHelpMessage = false;
        helpReceived = false;
        helpersLoad[0] = par("busVehicleLoad").doubleValue();
        helpersFreq[0] = par("randomCpuVehicleFreq").doubleValue();
        busIndex = 0;
        newRandomTime = 0;
        acceptingOtherVehicles = true;
        ackReceived = false;
        hostCpuFreq = 0;
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

    // SECTION - When the host receive load balancing message
    if (UpdateAvailabilityMessage* updateMsg = dynamic_cast<UpdateAvailabilityMessage*>(wsm)) {
        if (strcmp(updateMsg->getAvailability(), "not-available") == 0) {
            acceptingOtherVehicles = false;
        } else if (strcmp(updateMsg->getAvailability(), "available") == 0) {
            acceptingOtherVehicles = true;
            findHost()->getDisplayString().setTagArg("i", 1, "white");
        }
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

    // SECTION - When the bus receive the ACK message
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
        acceptingOtherVehicles = false;

        // Notify other vehicles of BUS balance loading
        UpdateAvailabilityMessage* updateMsg = new UpdateAvailabilityMessage();
        populateWSM(updateMsg);
        updateMsg->setAvailability("not-available");
        sendDown(updateMsg);

        balanceLoad(loadBalanceMsg->getSimulationTime());
    }

    // Timer for re-send ACK messages
    if (AckTimerMessage* ackTimerMsg = dynamic_cast<AckTimerMessage*>(msg)) {
        if (!ackReceived) {
            sendAgainResponse(ackTimerMsg->getHostIndex());
        }
    }

    // Timer for data computation
    if (ComputationTimerMessage* computationTimerMsg = dynamic_cast<ComputationTimerMessage*>(msg)) {
        sendAgainData(computationTimerMsg->getIndexHost(), computationTimerMsg->getLoadHost());
    }

    // Timer for ok message
    if (OkMessage* okMsg = dynamic_cast<OkMessage*>(msg)) {
        if (acceptingOtherVehicles) {
            // Color the available host in blue
            findHost()->getDisplayString().setTagArg("i", 1, "blue");

            // Send the ok message
            sendDown(okMsg->dup());
        }
    }

    // Timer for data message
    if (DataMessage* dataMsg = dynamic_cast<DataMessage*>(msg)) {
        sendDown(dataMsg->dup());
    }

    // Timer for response message
    if (ResponseMessage* responseMsg = dynamic_cast<ResponseMessage*>(msg)) {
        findHost()->getDisplayString().setTagArg("i", 1, "white");
        sendDown(responseMsg->dup());
    }

    // Timer for ACK message
    if (AckMessage* ackMsg = dynamic_cast<AckMessage*>(msg)) {
        sendDown(ackMsg->dup());
    }
}

void VeinsApp::handleHelpMessage(HelpMessage* helpMsg)
{
    // Check that the help request come from the bus
    if (helpMsg->getVehicleIndex() == busIndex) {
        // Color the host that received the help message
        findHost()->getDisplayString().setTagArg("i", 1, "yellow");

        // Set for every single host a random value for current load and cpu freq
        double randomVehicleLoad = par("randomVehicleLoadActual").doubleValue();
        double maximumVehicleLoad = par("maximumVehicleLoadActual").doubleValue();
        double commonVehicleLoad = par("commonVehicleLoad").doubleValue();

        // Get the CPU freq and assign to host variable
        double cpuFreq = par("randomCpuVehicleFreq").doubleValue();
        hostCpuFreq = cpuFreq;

        // Check the random vehicle load is inferior to maximum
        // actual vehicle load for computation
        if (randomVehicleLoad < maximumVehicleLoad) {
            // Calculate real actual load
            // Actual load = common load - random actual load
            double actualLoad = commonVehicleLoad - randomVehicleLoad - 1e08;

            // If the host is available send an ok message after
            // some time with ID and the computation load available
            if (actualLoad > 0) {
                // Prepare the message
                OkMessage* okMsg = new OkMessage();
                populateWSM(okMsg);
                okMsg->setHostID(findHost()->getIndex());
                okMsg->setAvailableLoad(actualLoad);
                okMsg->setCpuFreq(cpuFreq);

                // Schedule the ok message
                scheduleAt(simTime() + 2 + uniform(2, 4), okMsg);
            }
        } else if (findHost()->getIndex() != busIndex) {
            findHost()->getDisplayString().setTagArg("i", 1, "white");
        }
    }
}

void VeinsApp::handleOkMessage(OkMessage* okMsg)
{
    if (findHost()->getIndex() == busIndex) {
        // Color the bus that received help
        findHost()->getDisplayString().setTagArg("i", 1, "green");

        // Store the helper load and CPU freq
        helpersLoad[okMsg->getHostID()] = okMsg->getAvailableLoad();
        helpersFreq[okMsg->getHostID()] = okMsg->getCpuFreq();
    }
}

void VeinsApp::balanceLoad(simtime_t previousSimulationTime)
{
    std::map<int, double>::iterator loadsIterator = helpersLoad.begin();
    int vehiclesCounter = helpersLoad.size();

    if (vehiclesCounter > 1) {
        helpReceived = true;

        while (loadsIterator != helpersLoad.end()) {
            // Check if I'm not the bus
            if (loadsIterator->first != busIndex) {
                // Check if there's data to process
                double data = par("computationLoad").doubleValue();
                double vehicleLoad = loadsIterator->second;

                // If there's data to load then send the messages
                if (data > 0) {
                    // Create Data Message
                    DataMessage* dataMsg = new DataMessage();

                    // Populate the message
                    populateWSM(dataMsg);
                    dataMsg->setHostIndex(loadsIterator->first);

                    // If data - vehicleLoad >= 0 then set new data, otherwise send the remaining data
                    if ((data - vehicleLoad) >= 0) {
                        data = data - vehicleLoad;
                        dataMsg->setLoadToProcess(loadsIterator->second);
                    } else {
                        data = 0;
                        dataMsg->setLoadToProcess(data);
                    }

                    EV << "Load remaining: " << data << std::endl;

                    // Schedule the data message
                    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), dataMsg);

                    // Update global parameter data
                    par("computationLoad").setDoubleValue(data);

                    // Create timer computation message for each host
                    ComputationTimerMessage* computationTimerMsg = new ComputationTimerMessage();
                    populateWSM(computationTimerMsg);
                    computationTimerMsg->setSimulationTime(simTime());
                    computationTimerMsg->setIndexHost(loadsIterator->first);
                    computationTimerMsg->setLoadHost(loadsIterator->second);

                    // Calculate time for timer
                    double CPI = 3;
                    double I = loadsIterator->second;
                    double CR = helpersFreq[loadsIterator->first];

                    double timeToCompute = CPI * I * (1 / CR);

                    scheduleAt(simTime() + timeToCompute + uniform(5, 10), computationTimerMsg);
                }
            }

            // Increment the iterator
            loadsIterator++;
        }
    } else {
        sentHelpMessage = false;
        newRandomTime = previousSimulationTime;
    }
}

void VeinsApp::sendAgainData(int index, double load)
{
    auto found = helpersLoad.find(index);
    if (found != helpersLoad.end()) {
        // Prepare the new data message
        DataMessage* dataMsg = new DataMessage();
        populateWSM(dataMsg);
        dataMsg->setHostIndex(index);
        dataMsg->setLoadToProcess(load);
        sendDown(dataMsg);

        // Restart again the timer
        ComputationTimerMessage* computationTimerMsg = new ComputationTimerMessage();
        populateWSM(computationTimerMsg);
        computationTimerMsg->setSimulationTime(simTime());
        computationTimerMsg->setIndexHost(index);
        computationTimerMsg->setLoadHost(load);
        scheduleAt(simTime() + 10 + uniform(5, 10), computationTimerMsg);
    }
}

void VeinsApp::handleDataMessage(DataMessage* dataMsg)
{
    if (findHost()->getIndex() == dataMsg->getHostIndex()) {
        // Color the host that needs to process data
        findHost()->getDisplayString().setTagArg("i", 1, "red");

        EV << "Received " << dataMsg->getLoadToProcess() << " to load from BUS" << std::endl;

        // Calculate time for computation
        double CPI = 3;
        double I = dataMsg->getLoadToProcess();
        double CR = hostCpuFreq;

        double timeToCompute = CPI * I * (1 / CR);

        ResponseMessage* responseMsg = new ResponseMessage();
        populateWSM(responseMsg);
        responseMsg->setHostIndex(dataMsg->getHostIndex());
        scheduleAt(simTime() + timeToCompute + uniform(0.01, 0.2), responseMsg);

        AckTimerMessage* ackTimerMsg = new AckTimerMessage();
        populateWSM(ackTimerMsg);
        ackTimerMsg->setHostIndex(dataMsg->getHostIndex());
        scheduleAt(simTime() + 2 + uniform(3, 4), ackTimerMsg);

        EV << "Finished computation of: " << dataMsg->getLoadToProcess() << std::endl;
    }
}

void VeinsApp::handleResponseMessage(ResponseMessage* responseMsg)
{
    if (findHost()->getIndex() == busIndex) {
        helpersLoad.erase(responseMsg->getHostIndex());

        // Send ACK message to the host
        AckMessage* ackMsg = new AckMessage();
        populateWSM(ackMsg);
        ackMsg->setHostIndex(responseMsg->getHostIndex());
        scheduleAt(simTime() + 2 + uniform(1, 2), ackMsg);

        EV << "Deleted host: " << responseMsg->getHostIndex() << std::endl <<"Host remaining: " << helpersLoad.size() - 1 << std::endl;

        if (helpersLoad.size() == 1) {
            findHost()->getDisplayString().setTagArg("i", 1, "white");
            helpReceived = false;
            sentHelpMessage = false;
            newRandomTime = simTime();
            acceptingOtherVehicles = true;

            // Notify all vehicles of finished computation
            UpdateAvailabilityMessage* updateMsg = new UpdateAvailabilityMessage();
            populateWSM(updateMsg);
            updateMsg->setAvailability("available");
            sendDown(updateMsg);
        }
    }
}

void VeinsApp::sendAgainResponse(int index)
{
    ResponseMessage* responseMsg = new ResponseMessage();
    populateWSM(responseMsg);
    responseMsg->setHostIndex(index);
    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), responseMsg);
}

void VeinsApp::handlePositionUpdate(cObject* obj)
{
    // The vehicle has moved. Code that reacts to new positions goes here.
    // Member variables such as currentPosition and currentSpeed are updated in the parent class

    veins::DemoBaseApplLayer::handlePositionUpdate(obj);

    bool randomTimeReached = simTime() > par("randomTimeHelpMessage").doubleValue() + newRandomTime;
    bool isBus = findHost()->getIndex() == busIndex;
    bool notSentHelpMessage = !(sentHelpMessage);
    bool notHelpReceived = !(helpReceived);
    bool moreDataToLoad = par("computationLoad").doubleValue() > 0;

    if (randomTimeReached && isBus && notHelpReceived && notSentHelpMessage && moreDataToLoad) {
        // Help message creation
        HelpMessage* helpRequest = new HelpMessage();
        populateWSM(helpRequest);

        // Color the bus
        findHost()->getDisplayString().setTagArg("i", 1, "red");

        // Fill the data of the help request message
        helpRequest->setVehicleIndex(findHost()->getIndex());

        // Send the help message
        sendDown(helpRequest);

        // Schedule timer for the help request
        LoadBalanceTimerMessage* loadBalanceMsg = new LoadBalanceTimerMessage();
        populateWSM(loadBalanceMsg);
        loadBalanceMsg->setSimulationTime(simTime());
        scheduleAt(simTime() + 2 + uniform(5, 7), loadBalanceMsg);

        sentHelpMessage = true;
    } else if (!moreDataToLoad) {
        findHost()->getDisplayString().setTagArg("i", 1, "white");
    }

    lastDroveAt = simTime();
}
