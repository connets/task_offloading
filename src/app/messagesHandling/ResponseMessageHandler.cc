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

void TaskGenerator::handleResponseMessage(ResponseMessage* responseMsg)
{
    if (findHost()->getIndex() == busIndex) {
        responsesReceived++;
        EV << "Received data from host: " << responseMsg->getHostIndex() << std::endl;

        // Update global parameter data
        double data = par("computationLoad").doubleValue() - responseMsg->getDataComputed();

        EV << "Computed: " << responseMsg->getDataComputed() << std::endl << "Remaining: " << data << std::endl;

        if (!(par("useAcks").boolValue())) {
            // Send ACK message to the host
            AckMessage* ackMsg = new AckMessage();
            populateWSM(ackMsg);
            ackMsg->setHostIndex(responseMsg->getHostIndex());
            scheduleAt(simTime(), ackMsg);
        }

        if (!(responseMsg->getStillAvailable())) {
            helpers.erase(responseMsg->getHostIndex());
            helpersOrderedList.remove(responseMsg->getHostIndex());
            EV << "Deleted host: " << responseMsg->getHostIndex() << std::endl <<"Host remaining: " << helpers.size() - 1 << std::endl;

            // Check if there is more data to load at the end of the last response
            // message, to send signal of task terminated
            if(!(par("computationLoad").doubleValue() > 0)) {
                emit(stopTask, simTime());
            }

            // Send signal for having received response message statistic
            emit(stopResponseMessages, responseMsg->getHostIndex());

            if (helpers.size() == 1) {
                findHost()->getDisplayString().setTagArg("i", 1, "white");
                helpReceived = false;
                sentHelpMessage = false;
                newRandomTime = simTime();

                // Disable the load balancing mode
                loadBalancingState.setState(new Disabled);
            }
        }

        if (data > 0) {
            par("computationLoad").setDoubleValue(data);
        } else {
            par("computationLoad").setDoubleValue(0);

            // Reset ok and responses
            responsesReceived = 0;
            okReceived = 0;
        }

        // If the responses are equal to the total of vehicles update map and restart load balancing
        if (responsesReceived == okReceived) {
            EV << "Responses received: " << responsesReceived << std::endl;
            EV << "Ok received: " << okReceived << std::endl;
            responsesReceived = 0;

            if (helpers.size() > 1 && data > 0) {
                okReceived = helpers.size() - 1;

                // Increment the ID of load balancing
                loadBalancingID++;

                // Restart load balancing
                balanceLoad(simTime());
            } else {
                okReceived = 0;
            }

            EV << "Total data remaining: " << par("computationLoad").doubleValue() << std::endl;
        }
    }
}
