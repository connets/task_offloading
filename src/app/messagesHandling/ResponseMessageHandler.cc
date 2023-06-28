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
    // Search the vehicle in the map
    auto found = helpers.find(responseMessage->getHostIndex());

    // If the auto is found in the map and the partition id coincide with response message then
    // handle the response otherwise get rid of it
    if (found != helpers.end() && helpers[responseMessage->getHostIndex()].getDataPartitionId() == responseMessage->getPartitionID()) {
        // Emit signal for having received response
        emit(stopResponseMessages, responseMessage->getHostIndex());

        // Cancel and delete the timer message of this vehicle
        cancelAndDelete(helpers[responseMessage->getHostIndex()].getVehicleComputationTimer());

        // Update the data partiton id into the helpers map
        helpers[responseMessage->getHostIndex()].setDataPartitionId(-1);

        // Remove the data that the vehicle has computed
        double localData = tasks[0].getTotalData() - responseMessage->getDataComputed();
        tasks[0].setTotalData(localData);

        // If there's no more data then emit signal for task finished
        if (localData <= 0) {
            emit(stopTask, simTime());
        }

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
            // Schedule the ack message
            if (!(par("useAcks").boolValue())) {
                // Send ACK message to the host
                AckMessage* ackMessage = new AckMessage();
                populateWSM(ackMessage);
                ackMessage->setHostIndex(responseMessage->getHostIndex());
                ackMessage->setTaskID(responseMessage->getTaskID());
                ackMessage->setPartitionID(responseMessage->getPartitionID());
                ackMessage->setSenderAddress(mac->getMACAddress());
                ackMessage->setRecipientAddress(responseMessage->getSenderAddress());
                scheduleAt(simTime(), ackMessage);
            }

            helpers.erase(responseMessage->getHostIndex());
            helpersOrderedList.remove(responseMessage->getHostIndex());
        }

        // If there are more vehicles available and I've received all responses
        // then restart load balancing
        if (helpers.size() > 0 && localData > 0 && vehiclesAvailable == responseReceived) {
            // Increment load balance id
            loadBalanceId++;
            tasks[0].setLoadBalancingId(loadBalanceId);

            // Set the new availability
            int newAvailability = helpers.size();
            tasks[0].setAvailableReceivedCounter(newAvailability);

            // Set the responses received to 0
            tasks[0].setResponseReceivedCounter(0);

            balanceLoad();
        }

        // If there are no more vehicles but still more data to compute then take the bus
        // back in help status
        if (helpers.size() == 0 && localData > 0 && vehiclesAvailable == responseReceived) {
            // Color the bus in white when it has no more vehicles
            findHost()->getDisplayString().setTagArg("i", 1, "white");

            // Set the new availability
            tasks[0].setAvailableReceivedCounter(0);

            // Set the responses received to 0
            tasks[0].setResponseReceivedCounter(0);

            // Change it's status in help
            busState.setState(new Help);
        }
    } else if (tasks[0].getTotalData() <= 0) {
        // If data <= 0 and I receive a response then send ack to the vehicle

        // Schedule the ack message
        if (!(par("useAcks").boolValue())) {
            // Send ACK message to the host
            AckMessage* ackMessage = new AckMessage();
            populateWSM(ackMessage);
            ackMessage->setHostIndex(responseMessage->getHostIndex());
            ackMessage->setTaskID(responseMessage->getTaskID());
            ackMessage->setPartitionID(responseMessage->getPartitionID());
            ackMessage->setSenderAddress(mac->getMACAddress());
            ackMessage->setRecipientAddress(responseMessage->getSenderAddress());
            scheduleAt(simTime(), ackMessage);
        }

        helpers.erase(responseMessage->getHostIndex());
        helpersOrderedList.remove(responseMessage->getHostIndex());
    }
}
