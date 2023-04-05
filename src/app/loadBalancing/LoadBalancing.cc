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

#include "app/VeinsApp.h"
#include "app/messages/ComputationTimerMessage_m.h"

using namespace task_offloading;

void VeinsApp::balanceLoad(simtime_t previousSimulationTime)
{
    // Send signal for balance load
    emit(startBalance, simTime());

    // Send signal for stopping accepting help requests
    emit(stopHelp, simTime());

    int vehiclesCounter = helpers.size();
    helpersOrderedList = loadBalancingAlgorithm->sort(helpers);

    // Send signal for stop balance load
    emit(stopBalance, simTime());

    EV << "List ordered: ";

    for (auto const &i: helpersOrderedList) {
        EV << i << " ";
    }

    EV << std::endl;

    if (vehiclesCounter > 1) {
        helpReceived = true;

        // Check if there's data to process
        double data = par("computationLoad").doubleValue();

        for (auto const &i: helpersOrderedList) {
            // Check if the vehicle isn't the bus and if the response received are different from oks
            if (i != busIndex) {
                EV << "Index of vehicle: " << i << std::endl;

                // Load of vehicle i
                double vehicleLoad = helpers[i].getCurrentLoad();

                // If there's data to load then send the messages
                if (data > 0) {
                    // Create Data Message
                    DataMessage* dataMsg = new DataMessage();

                    // Populate the message

                    // If auto acks is active then populate wsm with the sender address
                    // otherwise populate it without address
                    if (par("useAcks").boolValue()) {
                        populateWSM(dataMsg, helpers[i].getAddress());
                    } else {
                        populateWSM(dataMsg);
                    }

                    dataMsg->setSenderAddress(myId);
                    dataMsg->setHostIndex(i);

                    // If data - vehicleLoad >= 0 then set data to maximum vehicle load
                    // otherwise send the remaining data
                    if ((data - vehicleLoad) >= 0) {
                        dataMsg->setLoadToProcess(vehicleLoad);
                        data = data - vehicleLoad;
                    } else {
                        dataMsg->setLoadToProcess(data);
                        data = 0;
                    }

                    EV << "Load remaining: " << data << std::endl;
                    EV << "Load vehicle " << i << " time to complete " << helpers[i].getCurrentLoad() << std::endl;
                    EV << "Load vehicle " << i << " arrival time " << helpers[i].getCreatedAt() << std::endl;
                    EV << "Index vehicle " << i << " value " << helpers[i].getIndex() << std::endl;

                    // Schedule the data message
                    scheduleAt(simTime(), dataMsg);

                    // Create timer computation message for each host if auto ACKs are disabled
                    if (!(par("useAcks").boolValue())) {
                        ComputationTimerMessage* computationTimerMsg = new ComputationTimerMessage();
                        populateWSM(computationTimerMsg);
                        computationTimerMsg->setSimulationTime(simTime());
                        computationTimerMsg->setIndexHost(i);
                        computationTimerMsg->setLoadHost(helpers[i].getCurrentLoad());
                        computationTimerMsg->setLoadBalancingID(loadBalancingID);

                        // Calculate time for timer
                        double CPI = par("vehicleCPI").intValue();
                        double timeToCompute = helpers[i].getTotalComputationTime(CPI);

                        computationTimerMsg->setTaskComputationTime(timeToCompute);

                        scheduleAt(simTime() + timeToCompute + par("dataComputationThreshold").doubleValue(), computationTimerMsg);
                    }
                }
            }
        }
    } else {
        sentHelpMessage = false;
        newRandomTime = previousSimulationTime;
    }
}
