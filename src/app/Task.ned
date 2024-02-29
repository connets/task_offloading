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

simple Task
{
    parameters:
        // Parameters of the task object
        int id = default(0);
        double totalData = default(10e6);
        double minimumLoadRequested = default(64e3);
        int computingDensity = default(1);
        int helpReceivedCounter = default(0);
        int dataPartitionId = default(0);
        int loadBalancingId = default(0);
        int availableReceivedCounter = default(0);
        int responseReceivedCounter = default(0);

    	@class(task_offloading::Task);
    	@display("i=block/app2");
}
