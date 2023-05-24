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
#include "app/messages/AckMessage_m.h"

using namespace task_offloading;

void TaskGenerator::handleResponseMessage(ResponseMessage* responseMessage)
{
    // Emit signal for having received response
    emit(stopResponseMessages, responseMessage->getHostIndex());

    // Update the data partiton id into the helpers map
    helpers[responseMessage->getHostIndex()].setDataPartitionId(-1);

    // Remove the data that the vehicle has computed
    double localData = tasks[0].getData() - responseMessage->getDataComputed();
    tasks[0].setData(localData);

    // Increment the task responses received
    int responseReceived = tasks[0].getResponseReceivedCounter();
    responseReceived++;
    tasks[0].setResponseReceivedCounter(responseReceived);

    // Get the availability received
    int vehiclesAvailable = tasks[0].getAvailableReceivedCounter();

    // Get the load balancing id
    int loadBalanceId = tasks[0].getLoadBalancingId();

    // If the vehicle is not available anymore erase it from the map
    // and from the list
    if (responseMessage->getStillAvailable() == false) {
        helpers.erase(responseMessage->getHostIndex());
        helpersOrderedList.remove(responseMessage->getHostIndex());

        // Schedule the ack message
        if (!(par("useAcks").boolValue())) {
            // Send ACK message to the host
            AckMessage* ackMessage = new AckMessage();
            populateWSM(ackMessage);
            ackMessage->setHostIndex(responseMessage->getHostIndex());
            ackMessage->setTaskID(responseMessage->getTaskID());
            ackMessage->setPartitionID(responseMessage->getPartitionID());
            scheduleAt(simTime(), ackMessage);
        }
    }

    // Check if I've received all responses from vehicles
    if (vehiclesAvailable == responseReceived) {
        // Increment load balance id and restart load balancing
        loadBalanceId++;
        tasks[0].setLoadBalancingId(loadBalanceId);

        // If there are more data to load and more vehicles in the map
        // then restart load balancing
        if (localData > 0 && helpers.size() > 0) {
            // Update the number of vehicles available
            tasks[0].setAvailableReceivedCounter(helpers.size());
            tasks[0].setResponseReceivedCounter(0);

            balanceLoad();
        }

        // If there's more data but no vehicles the change the state of
        // the bus and increment the load balance id
        if (localData > 0 && helpers.size() == 0) {
            // Update the number of vehicles available
            tasks[0].setAvailableReceivedCounter(0);
            tasks[0].setResponseReceivedCounter(0);

            busState.setState(new Help);

            // Color the bus in white
            findHost()->getDisplayString().setTagArg("i", 1, "white");
        }

        // If there's no more data then conclude the computation
        if (localData <= 0) {
            // Color the bus in white
            findHost()->getDisplayString().setTagArg("i", 1, "white");

            // Update the number of vehicles available
            tasks[0].setAvailableReceivedCounter(0);
            tasks[0].setResponseReceivedCounter(0);

            // Change the bus state
            busState.setState(new FinishedComputation);

            // Emit signal for task stopped
            emit(stopTask, simTime());
        }
    }
}
