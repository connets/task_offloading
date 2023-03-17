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

simple VeinsApp extends DemoBaseApplLayer 
{
    parameters:
        volatile double randomTimeHelpMessage @unit(s) = default(uniform(30s, 40s));
        volatile double randomVehicleLoadActual @unit(byte) = default(uniform(150MB, 400MB));
        volatile double randomCpuVehicleFreq @unit(Hz) = default(uniform(1.20GHz, 2.70GHz));
        double maximumVehicleLoadActual @unit(byte) = default(300MB);
        double commonVehicleLoad @unit(byte) = default(1GB);
        double busVehicleLoad @unit(byte) = default(4GB);
        double computationLoad @mutable @unit(byte) = default(5GB);
        bool useAcks = default(false);
        
        // General statistic for task time
        @signal[startTask](type = simtime_t);
        @signal[stopTask](type = simtime_t);
        @statistic[startTimeTask](source = startTask; record = count,mean,max?,vector?);
        @statistic[stopTimeTask](source = stopTask; record = count,mean,max?,vector?);
        
        // Statistics for balance loading
        @signal[startBalance](type = simtime_t);
        @signal[stopBalance](type = simtime_t);
        @statistic[startBalanceLoad](source = startBalance; record = count,mean,max?,vector?);
        @statistic[stopBalanceLoad](source = startBalance; record = count,mean,max?,vector?);
        
        // Statistics for collecting help requests
        @signal[startHelp](type = simtime_t);
        @signal[stopHelp](type = simtime_t);
        @statistic[startHelpRequests](source = startHelp; record = count,mean,max?,vector?);
        @statistic[stopHelpRequests](source = stopHelp; record = count,mean,max?,vector?);
        
        // Statistics for sending data messages
        @signal[startData](type = int);
        @signal[stopData](type = int);
        @statistic[startDataMessages](source = startData; record = count,mean,max?,vector?);
        @statistic[stopDataMessages](source = stopData; record = count,mean,max?,vector?);
        
        // Statistics for sending response messages
        @signal[startResponse](type = int);
        @signal[stopResponse](type = int);
        @statistic[startResponseMessages](source = startResponse; record = count,mean,max?,vector?);
        @statistic[stopResponseMessages](source = stopResponse; record = count,mean,max?,vector?);

    @class(task_offloading::VeinsApp);
    @display("i=block/app2");
}
