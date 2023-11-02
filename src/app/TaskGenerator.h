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

#pragma once

#include "veins_inet/veins_inet.h"

#include "app/Task.h"
#include "app/messages/HelpMessage_m.h"
#include "app/messages/AvailabilityMessage_m.h"
#include "app/messages/DataMessage_m.h"
#include "app/messages/ResponseMessage_m.h"
#include "app/vehiclesHandling/HelperVehicleInfo.h"
#include "app/loadBalancing/sortingAlgorithm/BaseSorting.h"
#include "loadBalancing/BusState.h"
#include "veins_inet/VeinsInetApplicationBase.h"
#include "inet/transportlayer/udp/UdpHeader_m.h"
#include "inet/networklayer/ipv4/Ipv4InterfaceData.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/ipv4/Ipv4.h"

using namespace omnetpp;

namespace task_offloading {

/**
 * @brief
 * This is a stub for a typical Veins application layer.
 * Most common functions are overloaded.
 * See MyVeinsApp.cc for hints
 *
 * @author David Eckhoff
 *
 */

class VEINS_INET_API TaskGenerator : public veins::VeinsInetApplicationBase {
public:
    void initialize(int stage) override;
    void finish() override;

private:
    // Simulations signals
    // SECTION - Task
    simsignal_t startTask;
    simsignal_t stopTask;

    // SECTION - BalanceLoad
    simsignal_t startBalance;
    simsignal_t stopBalance;

    // SECTION - Help requests collection
    simsignal_t startHelp;

    // SECTION - Data messages statistics
    simsignal_t startDataMessages;

    // SECTION - Response messages statistics
    simsignal_t stopResponseMessages;

    // SECTION - Beaconing messages statistics
    simsignal_t stopBeaconMessages;

protected:
    simtime_t lastDroveAt;
    std::map<int, HelperVehicleInfo> helpers;
    std::map<int, Task> tasks;
    std::list<int> helpersOrderedList;
    int busIndex;
    BusContext busState;
    BaseSorting* loadBalancingAlgorithm;

protected:
    void handleAvailabilityMessage(AvailabilityMessage* okMsg);
    void handleResponseMessage(ResponseMessage* responseMsg);
    void sendAgainData(DataMessage* data);
    void balanceLoad();
    void vehicleHandler();
    virtual void handleStartOperation(inet::LifecycleOperation* doneCallback) override;
    virtual void handleStopOperation(inet::LifecycleOperation* doneCallback) override;
    virtual void handleMessageWhenUp(inet::cMessage* msg) override;
};
}
