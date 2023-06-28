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

    auto key = std::pair<int,int>(dataMessage->getTaskId(),dataMessage->getPartitionId());

    //if the cache is not empty it resends the response message tied to this data message
    if(!isNewPartition(dataMessage)){
        sendAgainResponse(responseCache.at(key));
        return;
    }

    //reset the task availability timer
    resetTaskAvailabilityTimer(dataMessage->getTaskId());

    // Color the vehicle in red when computing
    findHost()->getDisplayString().setTagArg("i", 1, "red");

    // Update the partition ID
    currentDataPartitionId = dataMessage->getPartitionId();

    // Update if I'll be still available
    stillAvailableProbability = par("stillAvailableProbability").doubleValue() > par("stillAvailableThreshold").doubleValue();
    if(stillAvailableProbability) {
        setTaskAvailabilityTimer(dataMessage->getTaskId(), dataMessage->getTaskSize());
    }


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
    responseMessage->setStillAvailable(stillAvailableProbability);
    responseMessage->setDataComputed(dataMessage->getLoadToProcess());
    responseMessage->setTimeToCompute(timeToCompute);
    responseMessage->setTaskID(dataMessage->getTaskId());
    responseMessage->setPartitionID(dataMessage->getPartitionId());
    responseMessage->addByteLength(dataMessage->getLoadToProcess());
    responseMessage->setSenderAddress(mac->getMACAddress());
    responseMessage->setRecipientAddress(dataMessage->getSenderAddress());

    //Insert response message in response cache
    responseCache.insert(std::pair<std::pair<int, int>, ResponseMessage*>(key, responseMessage->dup()));

    // Schedule the response message
    scheduleAfter(timeToCompute, responseMessage);

    // Generate ACK timer if parameter useAcks is false
    // to achieve secure protocol manually and if I'm not still available
    if (!(par("useAcks").boolValue()) && !(stillAvailableProbability)) {
        AckTimerMessage* ackTimerMessage = new AckTimerMessage();
        populateWSM(ackTimerMessage);
        ackTimerMessage->setData(responseMessage->dup());

        // Calculate time to file transmission
        double transferTime = 10.0;

        scheduleAfter(timeToCompute + transferTime + par("ackMessageThreshold").doubleValue(), ackTimerMessage);
    }
}
