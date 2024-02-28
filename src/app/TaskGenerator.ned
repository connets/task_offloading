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
import org.car2x.veins.subprojects.veins_inet.VeinsInetApplicationBase;

simple TaskGenerator extends VeinsInetApplicationBase 
{
    parameters:
        // The random time for te first help message
        volatile double randomTimeFirstHelpMessage @unit(s) = default(uniform(30s, 40s));
        
        // The random time for the other help messages
        volatile double randomTimeHelpRequest @unit(s) = default(uniform(1s, 2s));
        
        // The minimum load requested accepted by bus
        double minimumVehicleLoadRequested @unit(byte) = default(300kB);
        
        // The bus load
        double busVehicleLoad @unit(byte) = default(4MB);
        
        // The total data to compute
        double computationLoad @mutable @unit(byte) = default(10MB);
        
        // Set this to true if you want to use automatic ack messages
        bool useAcks = default(false);
        
        // Bus time to wait for availability messages
        double busWaitingTimeForAvailability @unit(s) = default(uniform(100ms, 200ms));
        
        // Extra time threshold for data computation
        volatile double dataComputationThreshold @unit(s) = default(uniform(100ms, 200ms));
        
        // The load balancing sorting algorithm
        string sortingAlgorithm = default("FIFO");
        
        // Time that is used to check whether a car will be in the bus range in those next seconds
        double availabilityRangeTime = default(-1.0);
        
        // The input/output ratio
        double IOratio = default(1);
        
        // Refactor time
        double retryFactorTime = default(3);
                
        // Override default veins data length bits
        double dataLengthBits @unit(byte) = default(0bit);
        
        // Local port
        int servicePort = default(10020);
        
        // Maximum UDP byte length
        double UDPMaxLength @unit(byte) = default(65535B);
        
        // General statistic for task time
        @signal[task_started](type = simtime_t);
        @signal[task_stopped](type = simtime_t);
        @statistic[startTimeTask](source = task_started; record = vector,stats; interpolationmode = none);
        @statistic[stopTimeTask](source = task_stopped; record = vector,stats; interpolationmode = none);
        
        // Statistics for collecting beacon messages
        @signal[stopBeaconMessages](type = simtime_t);
        @statistic[stopBeaconMessages](source = stopBeaconMessages; record = vector,stats; interpolationmode = none);
        
        // Statistics for balance loading
        @signal[start_balance_loading](type = simtime_t);
        @signal[stop_balance_loading](type = simtime_t);
        @statistic[startBalanceLoad](source = start_balance_loading; record = vector,stats; interpolationmode = none);
        @statistic[stopBalanceLoad](source = stop_balance_loading; record = vector,stats; interpolationmode = none);
        
        // Statistics for collecting help requests
        @signal[start_bus_help_rq](type = simtime_t);
        @statistic[startHelpRequests](source = start_bus_help_rq; record = vector,stats; interpolationmode = none);
        
        // Statistics for sending data messages
        @signal[start_sending_data](type = int);
        @statistic[startDataMessages](source = start_sending_data; record = vector,stats; interpolationmode = none);
        
        // Statistics for sending response messages
        @signal[stop_getting_response](type = int);
        @statistic[stopResponseMessages](source = stop_getting_response; record = vector,stats; interpolationmode = none);

    	@class(task_offloading::TaskGenerator);
    	@display("i=block/app2");
}
