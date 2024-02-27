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
using namespace inet;

Define_Module(task_offloading::Beaconer);

void Beaconer::initialize(int stage)
{
    veins::VeinsInetApplicationBase::initialize(stage);

    if (stage == 0) {
       // Initializing members and pointers of your application goes here
       lastDroveAt = simTime();

      // Registering signal
      startBeaconMessages = registerSignal("startBeaconMessages");
    }
    sendBeaconEvt = new cMessage("beacon_evt", SEND_BEACON_EVT);
    // Get the timer for the first help message
    bool timerForFirstBeaconMessage = simTime() > par("randomWarmupTime").doubleValue();

    // This if was in logical AND with `sendBeacons`... Maybe an error?
    if(timerForFirstBeaconMessage) {
        // Emit signal for start beacon message
        emit(startBeaconMessages, simTime());

        if(sendBeaconEvt->isScheduled()){
             cancelAndDelete(sendBeaconEvt);
        }

        // Schedule the message -> simTime + availability msgs threshold
        scheduleAt(simTime(), sendBeaconEvt);

    }
}

void Beaconer::handleStartOperation(inet::LifecycleOperation* doneCallback)
{
    // Starting operations for this app
}

void Beaconer::handleStopOperation(inet::LifecycleOperation* doneCallback)
{
    // Close the socket
    socket.close();
}

void Beaconer::finish()
{
    veins::VeinsInetApplicationBase::finish();
}

/* This method doesn't exists in veins_inet
void Beaconer::onBSM(veins::DemoSafetyMessage* bsm)
{
    emit(stopBeaconMessages, getParentModule()->getIndex());
}
*/

void Beaconer::handleMessageWhenUp(inet::cMessage* msg)
{
    // Check if the message is a self message
    if (msg->isSelfMessage()) {
        switch (msg->getKind()) {
            case SEND_BEACON_EVT: {
                /*
                if(msg->isScheduled()){
                    cancelEvent(msg);
                }
                */

                if(simTime() >= par("randomWarmupTime").doubleValue()) {
                    // Create payload
                    auto bsm = makeShared<BeaconMessage>();
                    bsm->setChunkLength(B(par("beaconByteLength").doubleValue()));
                    timestampPayload(bsm);

                    // Create UDP header
                    auto udpHeader = makeShared<UdpHeader>();

                    // Create packet
                    auto packet = createPacket("bsm");
                    packet->insertAtBack(bsm);
                    packet->insertAtFront(udpHeader);
                    sendPacket(std::move(packet));

                    // Emit signal for start beacon message
                    emit(startBeaconMessages, simTime());

                }
                // Schedule the message -> simTime + availability msgs threshold
                // scheduleAt(simTime() + par("beaconIntervalTime").doubleValue(), sendBeaconEvt);
                break;
            }
            default: {
                if (msg) EV_WARN << "APP: Error: Got Self Message of unknown kind! Name: " << msg->getName() << endl;
                break;
            }
        }
    }
}
