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

#include "app/Worker.h"
#include "app/messages/AckTimerMessage_m.h"

using namespace task_offloading;

void Worker::handleDataMessage(DataMessage* dataMessage)
{
    // Emit the signal for have received data message
    emit(stopDataMessages, dataMessage->getHostIndex());

    // Calculate time for computation
    double CPI = dataMessage->getCpi();
    double I = dataMessage->getLoadToProcess();
    double CR = cpuFreq;

    double timeToCompute = CPI * I * (1 / CR);

    // Calculate probability to be still available after computation
    bool stillAvailable = par("stillAvailableProbability").doubleValue() > par("stillAvailableThreshold").doubleValue();

    // Check if I'm the vehicle designated for computation and if the data is different
    if (dataMessage->getHostIndex() == findHost()->getIndex()) {
        // Color the vehicle in red when computing
        findHost()->getDisplayString().setTagArg("i", 1, "red");

        // Update the partition ID
        currentDataPartitionId = dataMessage->getPartitionID();

        // Prepare the response message
        ResponseMessage* responseMessage = new ResponseMessage();

        // Populate the response message

        // If useAcks is active then send the message with L2Address
        // otherwise send it without address to use the manual secure protocol
        if (par("useAcks").boolValue()) {
            populateWSM(responseMessage, dataMessage->getSenderAddress());
        } else {
            populateWSM(responseMessage);
        }

        // Populate other fields
        responseMessage->setHostIndex(dataMessage->getHostIndex());
        responseMessage->setStillAvailable(stillAvailable);
        responseMessage->setDataComputed(dataMessage->getLoadToProcess());
        responseMessage->setTaskID(dataMessage->getTaskID());
        responseMessage->setPartitionID(dataMessage->getPartitionID());
        responseMessage->addByteLength(dataMessage->getLoadToProcess());

        // Schedule the response message
        scheduleAt(simTime() + timeToCompute, responseMessage);

        // Generate ACK timer if parameter useAcks is false
        // to achieve secure protocol manually and if I'm not still available
        if (!(par("useAcks").boolValue()) && !(stillAvailable)) {
            AckTimerMessage* ackTimerMessage = new AckTimerMessage();
            populateWSM(ackTimerMessage);
            ackTimerMessage->setHostIndex(dataMessage->getHostIndex());
            ackTimerMessage->setTaskComputationTime(timeToCompute);
            ackTimerMessage->setTaskID(dataMessage->getTaskID());
            ackTimerMessage->setPartitionID(currentDataPartitionId);

            // Calculate time to file transmission
            double transferTime = (dataMessage->getLoadToProcess() * 8) / 6;

            scheduleAt(simTime() + timeToCompute + transferTime + par("ackMessageThreshold").doubleValue(), ackTimerMessage);
        }
    }
}
