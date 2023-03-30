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
#include "app/messages/AckTimerMessage_m.h"

using namespace task_offloading;

void VeinsApp::sendAgainResponse(int index)
{
    if (!ackReceived) {
        ResponseMessage* responseMsg = new ResponseMessage();
        populateWSM(responseMsg);
        responseMsg->setHostIndex(index);
        scheduleAt(simTime() + 2 + uniform(0.01, 0.2), responseMsg);

        // Restart the ACK timer
        AckTimerMessage* ackTimerMsg = new AckTimerMessage();
        populateWSM(ackTimerMsg);
        ackTimerMsg->setHostIndex(index);
        scheduleAt(simTime() + 2 + uniform(3, 4), ackTimerMsg);
    }
}
