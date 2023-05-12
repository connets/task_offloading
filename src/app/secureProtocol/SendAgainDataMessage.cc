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

void TaskGenerator::sendAgainData(int index, double load, double taskComputationTime, int loadBalanceProgressiveNumber)
{
    auto found = helpers.find(index);
    if (found != helpers.end() && (loadBalancingID == loadBalanceProgressiveNumber)) {
        // Prepare the new data message
        DataMessage* dataMsg = new DataMessage();
        populateWSM(dataMsg);
        dataMsg->setHostIndex(index);
        dataMsg->setLoadToProcess(load);
        sendDown(dataMsg);

        // Restart again the timer
        ComputationTimerMessage* computationTimerMsg = new ComputationTimerMessage();
        populateWSM(computationTimerMsg);
        computationTimerMsg->setSimulationTime(simTime());
        computationTimerMsg->setIndexHost(index);
        computationTimerMsg->setLoadHost(load);
        scheduleAt(simTime() + taskComputationTime + par("dataComputationThreshold").doubleValue(), computationTimerMsg);
    }
}
