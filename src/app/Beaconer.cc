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

#include "Beaconer.h"

#include "app/messages/BeaconMessage_m.h"
using namespace task_offloading;

Define_Module(task_offloading::Beaconer);

void Beaconer::initialize(int stage)
{
    veins::DemoBaseApplLayer::initialize(stage);

    if (stage == 0) {
       // Initializing members and pointers of your application goes here
       lastDroveAt = simTime();

      //registering signal
      startBeaconMessages = registerSignal("startBeaconMessages");
    }
    sendBeaconEvt = new cMessage("beacon evt", SEND_BEACON_EVT);
    // Get the timer for the first help message
    bool timerForFirstBeaconMessage = simTime() > par("randomWarmupTime").doubleValue();

    if(timerForFirstBeaconMessage && sendBeacons) {
        // Emit signal for start beacon message
        emit(startBeaconMessages, simTime());

        // Save the actual simtime for future help messages
        simtime_t simTimeActual = simTime();

        // Schedule the message -> simTime + availability msgs threshold
        scheduleAt(simTimeActual + par("beaconIntervalTime").doubleValue(), sendBeaconEvt);

    }
}
void Beaconer::finish()
{
    veins::DemoBaseApplLayer::finish();
}

void Beaconer::onBSM(veins::DemoSafetyMessage* bsm)
{
    emit(stopBeaconMessages, findHost()->getIndex());
}

void Beaconer::onWSM(veins::BaseFrame1609_4* wsm)
{

}

void Beaconer::onWSA(veins::DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from a bus
}

void Beaconer::handleSelfMsg(cMessage* msg)
{
    switch (msg->getKind()) {
    case SEND_BEACON_EVT: {
        BeaconMessage* bsm = new BeaconMessage();

        if(simTime() >= par("randomWarmupTime").doubleValue()) {
            bsm->addByteLength(par("beaconByteLength").doubleValue());
            populateWSM(bsm);
            sendDown(bsm);
            // Emit signal for start beacon message
            emit(startBeaconMessages, simTime());

        }
        // Schedule the message -> simTime + availability msgs threshold
        scheduleAt(simTime() + par("beaconIntervalTime").doubleValue(), sendBeaconEvt);


        break;
    }
    default: {
        if (msg) EV_WARN << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
        break;
    }
    }
}
void Beaconer::handlePositionUpdate(cObject* obj)
{
    // The vehicle has moved. Code that reacts to new positions goes here.
    // Member variables such as currentPosition and currentSpeed are updated in the parent class
    veins::DemoBaseApplLayer::handlePositionUpdate(obj);

    lastDroveAt = simTime();
}
