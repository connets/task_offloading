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
        volatile double randomVehicleLoadActual = default(uniform(0, 1));
        volatile double randomCpuVehicleFreq @unit(Hz) = default(uniform(1.20GHz, 2.70GHz));
        double minimumVehicleLoadActual @unit(byte) = default(300kB);
        double commonVehicleLoad @unit(byte) = default(1MB);
        bool useAcks = default(false);
        volatile double stillAvailableProbability = default(uniform(0, 1));
        double stillAvailableThreshold = default(0.5);
        double vehicleOkMessageTime @unit(s) = default(uniform(0ms, 100ms));
        volatile double ackMessageThreshold @unit(s) = default(uniform(50ms, 100ms));
        int vehicleCPI = default(3);
        
        // Override default veins data length bits
        dataLengthBits = 0bit;
        
        // General statistic for task time
        @signal[task_started](type = simtime_t);
        @signal[task_stopped](type = simtime_t);
        @statistic[startTimeTask](source = task_started; record = vector,stats; interpolationmode = none);
        @statistic[stopTimeTask](source = task_stopped; record = vector,stats; interpolationmode = none);
        
        // Statistics for balance loading
        @signal[start_balance_loading](type = simtime_t);
        @signal[stop_balance_loading](type = simtime_t);
        @statistic[startBalanceLoad](source = start_balance_loading; record = vector,stats; interpolationmode = none);
        @statistic[stopBalanceLoad](source = stop_balance_loading; record = vector,stats; interpolationmode = none);
        
        // Statistics for collecting help requests
        @signal[start_bus_help_rq](type = simtime_t);
        @signal[stop_bus_help_rq](type = simtime_t);
        @statistic[startHelpRequests](source = start_bus_help_rq; record = vector,stats; interpolationmode = none);
        @statistic[stopHelpRequests](source = stop_bus_help_rq; record = vector,stats; interpolationmode = none);
        
        // Statistics for sending data messages
        @signal[start_sending_data](type = int);
        @signal[stop_sending_data](type = int);
        @statistic[startDataMessages](source = start_sending_data; record = vector,stats; interpolationmode = none);
        @statistic[stopDataMessages](source = stop_sending_data; record = vector,stats; interpolationmode = none);
        
        // Statistics for sending response messages
        @signal[start_getting_response](type = int);
        @signal[stop_getting_response](type = int);
        @statistic[startResponseMessages](source = start_getting_response; record = vector,stats; interpolationmode = none);
        @statistic[stopResponseMessages](source = stop_getting_response; record = vector,stats; interpolationmode = none);
        
        // Statistics for sending and load of OK messages
        @signal[ok_message_sent](type = simtime_t);
        @signal[ok_message_load](type = double);
        @statistic[okMessageSent](source = ok_message_sent; record = vector,stats; interpolationmode = none);
        @statistic[okMessageLoad](source = ok_message_load; record = vector,stats; interpolationmode = none);

    @class(task_offloading::Worker);
    @display("i=block/app2");
}
