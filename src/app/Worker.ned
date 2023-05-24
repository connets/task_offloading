//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package task_offloading.app;
import org.car2x.veins.modules.application.ieee80211p.DemoBaseApplLayer;

simple Worker extends DemoBaseApplLayer 
{
    parameters:
        // The random CPU frequency
        volatile double randomVehicleCpuFreq @unit(Hz) = default(uniform(1.20GHz, 2.70GHz));
        
        // The random percentage of free vehicle load
        volatile double randomVehicleFreeLoadPercentage = default(uniform(0, 1));
        
        // The common vehicle load
        double commonVehicleLoad @unit(byte) = default(1MB);
        
        // Set this to true if you want to use automatic ack messages
        bool useAcks = default(false);
        
        // The probability to be still available after computation
        volatile double stillAvailableProbability = default(uniform(0, 1));
        
        // The threshold for the above probability
        double stillAvailableThreshold = default(0.5);
        
        // The random time for the availability message
        double vehicleAvailabilityMessageTime @unit(s) = default(uniform(0ms, 100ms));
        
        // The threshold for the ack messages
        volatile double ackMessageThreshold @unit(s) = default(uniform(50ms, 100ms));
        
        // Override default veins data length bits
        dataLengthBits = 0bit;
        
        // Statistics for collecting help requests
        @signal[stop_bus_help_rq](type = simtime_t);
        @statistic[stopHelpRequests](source = stop_bus_help_rq; record = vector,stats; interpolationmode = none);
        
        // Statistics for sending data messages
        @signal[stop_sending_data](type = int);
        @statistic[stopDataMessages](source = stop_sending_data; record = vector,stats; interpolationmode = none);
        
        // Statistics for sending response messages
        @signal[start_getting_response](type = int);
        @statistic[startResponseMessages](source = start_getting_response; record = vector,stats; interpolationmode = none);
        
        // Statistics for sending and load of OK messages
        @signal[available_message_sent](type = simtime_t);
        @signal[available_message_load](type = double);
        @statistic[availableMessageSent](source = available_message_sent; record = vector,stats; interpolationmode = none);
        @statistic[availableMessageLoad](source = available_message_load; record = vector,stats; interpolationmode = none);

    @class(task_offloading::Worker);
    @display("i=block/app2");
}
