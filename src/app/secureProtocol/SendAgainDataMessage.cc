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

#include "app/TaskGenerator.h"
#include "app/messages/ComputationTimerMessage_m.h"

using namespace task_offloading;

void TaskGenerator::sendAgainData(const DataMessage* data)
{
    // Search the vehicle in the map
    auto found = helpers.find(data->getHostIndex());

    // Check load balancing id
    bool loadBalancingIdCheck = tasks[0].getLoadBalancingCounter() == data->getLoadBalancingId();

    // If the vehicle is found check if I've received the data from it
    if (found != helpers.end() && (loadBalancingIdCheck)) {
        // Check the data partition id
        bool checkDataPartitionId = helpers[data->getHostIndex()].getDataPartitionId() != -1;

        if (checkDataPartitionId) {
            // Send the duplicate data message
            sendDown(data->dup());

            // Restart again the timer
            ComputationTimerMessage* computationTimerMessage = new ComputationTimerMessage();
            populateWSM(computationTimerMessage);
            computationTimerMessage->setData(data->dup());

            double transferTime = 10.0;

            scheduleAt(simTime() + transferTime + data->getComputationTime() + par("dataComputationThreshold").doubleValue(), computationTimerMessage);
        }
    }
}
