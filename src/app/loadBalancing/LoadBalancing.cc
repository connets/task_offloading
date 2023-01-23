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

using namespace tirocinio;

void VeinsApp::balanceLoad(simtime_t previousSimulationTime)
{
    std::map<int, double>::iterator loadsIterator = helpersLoad.begin();
    int vehiclesCounter = helpersLoad.size();

    if (vehiclesCounter > 1) {
        helpReceived = true;

        while (loadsIterator != helpersLoad.end()) {
            // Check if I'm not the bus
            if (loadsIterator->first != busIndex) {
                // Check if there's data to process
                double data = par("computationLoad").doubleValue();
                double vehicleLoad = loadsIterator->second;

                // If there's data to load then send the messages
                if (data > 0) {
                    // Create Data Message
                    DataMessage* dataMsg = new DataMessage();

                    // Populate the message

                    // If auto acks is active then populate wsm with the sender address
                    // otherwise populate it without address
                    if (par("useAcks").boolValue()) {
                        populateWSM(dataMsg, helpersAddresses[loadsIterator->first]);
                    } else {
                        populateWSM(dataMsg);
                    }

                    dataMsg->setSenderAddress(myId);
                    dataMsg->setHostIndex(loadsIterator->first);

                    // If data - vehicleLoad >= 0 then set new data, otherwise send the remaining data
                    if ((data - vehicleLoad) >= 0) {
                        data = data - vehicleLoad;
                        dataMsg->setLoadToProcess(loadsIterator->second);
                    } else {
                        data = 0;
                        dataMsg->setLoadToProcess(data);
                    }

                    EV << "Load remaining: " << data << std::endl;

                    // Schedule the data message
                    scheduleAt(simTime() + 2 + uniform(0.01, 0.2), dataMsg);

                    // Update global parameter data
                    par("computationLoad").setDoubleValue(data);

                    // Create timer computation message for each host if auto ACKs are disabled
                    if (!(par("useAcks").boolValue())) {
                        ComputationTimerMessage* computationTimerMsg = new ComputationTimerMessage();
                        populateWSM(computationTimerMsg);
                        computationTimerMsg->setSimulationTime(simTime());
                        computationTimerMsg->setIndexHost(loadsIterator->first);
                        computationTimerMsg->setLoadHost(loadsIterator->second);

                        // Calculate time for timer
                        double CPI = 3;
                        double I = loadsIterator->second;
                        double CR = helpersFreq[loadsIterator->first];

                        double timeToCompute = CPI * I * (1 / CR);

                        scheduleAt(simTime() + timeToCompute + uniform(5, 10), computationTimerMsg);
                    }
                }
            }

            // Increment the iterator
            loadsIterator++;
        }
    } else {
        sentHelpMessage = false;
        newRandomTime = previousSimulationTime;
    }
}
