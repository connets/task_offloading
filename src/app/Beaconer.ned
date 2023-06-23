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

package task_offloading.app;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple Beaconer extends DemoBaseApplLayer 
{
    parameters:
        // Set this to true if you want to use automatic ack messages
        bool useAcks = default(false);
        // The random time for te first beacon message
        double randomWarmupTime @unit(s) = default(uniform(30s, 40s));
        //the length of a beacon packet
        double beaconByteLength @unit(byte) = default(uniform(100 byte)); 
        // The time the Beaconer has to wait before sending each message
        double beaconIntervalTime @unit(s) = default(uniform(100ms, 200ms)); 
        // Statistics for collecting beacon messages
        @signal[startBeaconMessages](type = simtime_t);
        @statistic[startBeaconMessages](source = startBeaconMessages; record = vector,stats; interpolationmode = none);
        
        @signal[stopBeaconMessages](type = simtime_t);
        @statistic[stopBeaconMessages](source = stopBeaconMessages; record = vector,stats; interpolationmode = none);

    @class(task_offloading::Beaconer);
    @display("i=block/app2");
}
