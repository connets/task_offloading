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

#include "app/HelpMessage_m.h"

using namespace tirocinio;

Define_Module(tirocinio::VeinsApp);

void VeinsApp::initialize(int stage)
{
    veins::DemoBaseApplLayer::initialize(stage);
    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        // EV << "Initializing " << par("appName").stringValue() << std::endl;
        sentHelpMessage = false;
        lastDroveAt = simTime();
        currentSubscribedServiceId = -1;
        helpReceived = false;
        helpOffered = false;
        vehicleLoad = rand() % 30;
        helperHostIndex = -1;
        connectionEstablished = false;
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void VeinsApp::finish()
{
    veins::DemoBaseApplLayer::finish();
    // statistics recording goes here
}

void VeinsApp::onBSM(veins::DemoSafetyMessage* bsm)
{
    // Your application has received a beacon message from another car or RSU
    // code for handling the message goes here
}

void VeinsApp::onWSM(veins::BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    HelpMessage* msg = check_and_cast<HelpMessage*>(wsm);

    // Receive first WSM from BUS
    if ((findHost()->getIndex() != 0) && (msg->getHelpReceived() == false) && (msg->getHelpOffered() == false)) {
        // Offer help only if vehicle load is inferior to a certain threshold
        if (vehicleLoad < msg->getVehicleLoad()) {
            // Blue color to indicate vehicle ready to help with appropriate vehicle load
            findHost()->getDisplayString().setTagArg("i", 1, "blue");

            // Set msg fields
            msg->setMsgContent("Ready to help!");
            msg->setSenderAddress(myId);
            msg->setHelperHostIndex(findHost()->getIndex());

            // Set new vehicle load
            msg->setVehicleLoad(vehicleLoad);

            // Set offer helped for this host
            helpOffered = true;
            scheduleAt(simTime() + 2 + uniform(0.01, 0.2), msg->dup());
        }
    }

    // If BUS receive WSM then save the host inside it's structure
    if ((findHost()->getIndex() == 0) && (msg->getHelpReceived() == false)) {
        // Check if vehicle load is inferior then save new vehicle load
        if (msg->getVehicleLoad() < vehicleLoad) {
            vehicleLoad = msg->getVehicleLoad();

            // And then save also the host index
            helperHostIndex = msg->getHelperHostIndex();

            // Set help received at least from one host
            helpReceived = true;
        }
    }

    // In this final section if we're receiving an ACK update all vehicles
    // and set the one we choose for collaboration
    if ((findHost()->getIndex() != 0) && (msg->getAck()) && (findHost()->getIndex() == msg->getHelperHostIndex())) {
        findHost()->getDisplayString().setTagArg("i", 1, "green");
    } else if ((findHost()->getIndex() != 0) && (msg->getAck()) && (findHost()->getIndex() != msg->getHelperHostIndex())) {
        findHost()->getDisplayString().setTagArg("i", 1, "white");
    }
}

void VeinsApp::onWSA(veins::DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void VeinsApp::handleSelfMsg(cMessage* msg)
{
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    HelpMessage* wsm = dynamic_cast<HelpMessage*>(msg);

    // If vehicle is different from BUS then send the help message
    if ((findHost()->getIndex() != 0) && (wsm->getHelpReceived() == false)) {
        // Set help offered to true
        wsm->setHelpOffered(true);
        sendDown(wsm);
    }
}

void VeinsApp::handlePositionUpdate(cObject* obj)
{
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class

    veins::DemoBaseApplLayer::handlePositionUpdate(obj);

    // Calculate random time
    auto randTime = rand() % 2000;

    // If random time limit reached send help request
    if ((findHost()->getIndex() == 0) && ((randTime - simTime()) < 0) && (helpReceived == false)) {
        // Set color to yellow
        findHost()->getDisplayString().setTagArg("i", 1, "yellow");

        sentHelpMessage = true;

        // Prepare help message
        HelpMessage* helpMsg = new HelpMessage();
        helpMsg->setMsgContent("Help needed!");
        helpMsg->setSenderAddress(myId);
        helpMsg->setHelpReceived(false);
        helpMsg->setAck(false);

        auto randomLoad = rand() % 30;
        vehicleLoad = randomLoad;
        helpMsg->setVehicleLoad(randomLoad);

        // Use only CCH
        helpMsg->setChannelNumber(static_cast<int>(veins::Channel::cch));

        // Send message
        sendDown(helpMsg);
    } else if ((mobility->getSpeed() > 0) && (findHost()->getIndex() == 0) && (helpReceived) && (!connectionEstablished)) {
        // If we're good to go with the data collected from other vehicles send
        // a message to confirm with which vehicle we're collaborating

        // Set color to green
        findHost()->getDisplayString().setTagArg("i", 1, "green");

        // Set successful connection
        connectionEstablished = true;

        // Prepare ACK message
        HelpMessage* ACK = new HelpMessage();
        ACK->setMsgContent("ACK");
        ACK->setSenderAddress(myId);
        ACK->setHelpReceived(true);

        // Set the correct host that's helping us
        ACK->setHelperHostIndex(helperHostIndex);
        ACK->setAck(true);

        // Use only CCH
        ACK->setChannelNumber(static_cast<int>(veins::Channel::cch));

        // Send message
        sendDown(ACK);
    } else {
        lastDroveAt = simTime();
    }
}
