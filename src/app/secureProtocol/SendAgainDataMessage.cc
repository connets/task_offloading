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

void TaskGenerator::sendAgainData(DataMessage* data)
{
    // Search the vehicle in the map
    auto found = helpers.find(data->getHostIndex());

    // Check load balancing id
    bool loadBalancingIdCheck = tasks[0].getLoadBalancingId() == data->getLoadBalancingId();

    // If the vehicle is found check if I've received the data from it
    if (found != helpers.end() && (loadBalancingIdCheck)) {
        // Check the data partition id
        bool checkDataPartitionId = helpers[data->getHostIndex()].getDataPartitionId() != -1;

        if (checkDataPartitionId) {
            DataMessage* newData = new DataMessage();

            // Fill fields of data message with previous data message

            /**************************************************************************
             * This has to be done because in veins if you send a message duplicate   *
             * it will be discarded from MAC L2 because it's a message that the       *
             * single node "read" as already received                                 *
             *************************************************************************/

            populateWSM(newData);
            newData->addByteLength(data->getByteLength());
            newData->setLoadToProcess(data->getLoadToProcess());
            newData->setSenderAddress(mac->getMACAddress());
            newData->setHostIndex(data->getHostIndex());
            newData->setTaskId(data->getTaskId());
            newData->setTaskSize(data->getTaskSize());
            newData->setPartitionId(data->getPartitionId());
            newData->setLoadBalancingId(data->getLoadBalancingId());
            newData->setCpi(data->getCpi());
            newData->setRecipientAddress(data->getRecipientAddress());
            newData->setComputationTime(data->getComputationTime());

            // Send the duplicate data message
            sendDown(newData);

            // Restart again the timer
            ComputationTimerMessage* computationTimerMessage = new ComputationTimerMessage();
            populateWSM(computationTimerMessage);
            computationTimerMessage->setData(data->dup());

            double transferTime = 10.0;

            scheduleAt(simTime() + transferTime + data->getComputationTime() + par("dataComputationThreshold").doubleValue(), computationTimerMessage);
        }
    }
}
