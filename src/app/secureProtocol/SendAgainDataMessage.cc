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

void TaskGenerator::sendAgainData(int index, double load, double taskComputationTime, int loadBalanceProgressiveNumber, int previousTaskID, int previousPartitionID)
{
    // Search the vehicle in the map
    auto found = helpers.find(index);

    // Check load balancing id
    bool loadBalancingIdCheck = tasks[0].getLoadBalancingId() == loadBalanceProgressiveNumber;

    // Check the data partition id
    bool checkDataPartitionId = helpers[index].getDataPartitionId() == previousPartitionID;

    // If the vehicle is found check if I've received the data from it
    if (found != helpers.end() && (loadBalancingIdCheck) && (checkDataPartitionId)) {
        // Prepare the new data message
        DataMessage* dataMessage = new DataMessage();
        populateWSM(dataMessage);
        dataMessage->setHostIndex(index);
        dataMessage->setLoadToProcess(load);
        dataMessage->setTaskID(previousTaskID);
        dataMessage->setPartitionID(previousPartitionID);
        sendDown(dataMessage);

        // Restart again the timer
        ComputationTimerMessage* computationTimerMessage = new ComputationTimerMessage();
        populateWSM(computationTimerMessage);
        computationTimerMessage->setSimulationTime(simTime());
        computationTimerMessage->setIndexHost(index);
        computationTimerMessage->setLoadHost(load);
        computationTimerMessage->setTaskID(previousTaskID);
        computationTimerMessage->setPartitionID(previousPartitionID);
        scheduleAt(simTime() + taskComputationTime + par("dataComputationThreshold").doubleValue(), computationTimerMessage);
    }
}
