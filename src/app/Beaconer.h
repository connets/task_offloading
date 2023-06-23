//
// Copyright (C) 2016 David Eckhoff <eckhoff@cs.fau.de>
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

#include "veins/veins.h"

#include "app/vehiclesHandling/HelperVehicleInfo.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

using namespace omnetpp;

namespace task_offloading {

/**
 * @brief
 * Demo application layer base class.
 *
 * @author David Eckhoff
 *
 * @ingroup applLayer
 *
 * @see DemoBaseApplLayer
 * @see Mac1609_4
 * @see PhyLayer80211p
 * @see Decider80211p
 */
class VEINS_API Beaconer : public veins::DemoBaseApplLayer  {

public:
    void initialize(int stage) override;
    void finish() override;

    enum DemoApplMessageKinds {
            SEND_BEACON_EVT
        };
private:
    // Simulations signals
    simsignal_t startBeaconMessages;
    simsignal_t stopBeaconMessages;
protected:
    simtime_t lastDroveAt;
    volatile double beaconIntervalTime;
    double beaconByteLength;
    /* messages for periodic events such as beacon transmissions */
    cMessage* sendBeaconEvt;
protected:
    void onBSM(veins::DemoSafetyMessage* bsm) override;
    void onWSM(veins::BaseFrame1609_4* wsm) override;
    void onWSA(veins::DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
};
}
