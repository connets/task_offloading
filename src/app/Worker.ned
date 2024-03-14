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
import org.car2x.veins.subprojects.veins_inet.VeinsInetApplicationBase;

simple Worker extends VeinsInetApplicationBase
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
        
        // The input/output ratio
        double IOratio = default(1);
        
        //refactor time
        double retryFactorTime = default(3);
        
        // Override default veins data length bits
        double dataLengthBits @unit(byte) = default(0bit);
        
        // Local port
        int servicePort = default(10020);
        
        // Statistics for collecting beacon messages
        @signal[stopBeaconMessages](type = simtime_t);
        @statistic[stopBeaconMessages](source = stopBeaconMessages; record = vector,stats; interpolationmode = none);
        
        // Statistics for collecting retransmissions
        @signal[totalRetransmissionsSignal](type = int);
		@statistic[totalRetransmissions](title="Total number of retransmissions"; source="totalRetransmissionsSignal"; record=vector; interpolationmode=none);
        
        // Statistics for transmission time of each packet
        @signal[transmissionTimePacketSignal](type = double);
		@statistic[transmissionTimePacket](title="Transmission time of a packet"; source="transmissionTimePacketSignal"; record=vector; interpolationmode=none);
        
        // Statistics for transmission time of each chunk
        @signal[transmissionTimeChunkSignal](type = double);
		@statistic[transmissionTimeChunk](title="Transmission time of a chunk"; source="transmissionTimeChunkSignal"; record=vector; interpolationmode=none);
        
        @class(task_offloading::Worker);
    	@display("i=block/app2");
}
