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

void Worker::sendAgainResponse(ResponseMessage* response)
{
    if (currentDataPartitionId == response->getPartitionID()) {
        ResponseMessage* newResponse = new ResponseMessage();

        // Fill fields of response message with previous response message

        /**************************************************************************
         * This has to be done because in veins if you send a message duplicate   *
         * it will be discarded from MAC L2 because it's a message that the       *
         * single node "read" as already received                                 *
         *************************************************************************/

        populateWSM(newResponse);
        newResponse->setHostIndex(response->getHostIndex());
        newResponse->setStillAvailable(response->getStillAvailable());
        newResponse->setDataComputed(response->getDataComputed());
        newResponse->setTimeToCompute(response->getTimeToCompute());
        newResponse->setTaskID(response->getTaskID());
        newResponse->setPartitionID(response->getPartitionID());
        newResponse->addByteLength(response->getByteLength());
        newResponse->setSenderAddress(mac->getMACAddress());
        newResponse->setRecipientAddress(response->getRecipientAddress());

        // Schedule the new duplicate response message
        scheduleAfter(response->getTimeToCompute(), newResponse);

        // Restart the ACK timer
        AckTimerMessage* ackTimerMessage = new AckTimerMessage();
        populateWSM(ackTimerMessage);
        ackTimerMessage->setData(response->dup());

        double transferTime = 10.0;

        scheduleAt(simTime() + transferTime + response->getTimeToCompute() + par("ackMessageThreshold").doubleValue(), ackTimerMessage);
    }
}
