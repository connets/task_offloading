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
#include "app/messages/AckMessage_m.h"

using namespace tirocinio;

void VeinsApp::handleResponseMessage(ResponseMessage* responseMsg)
{
    if (findHost()->getIndex() == busIndex && (!(par("useAcks").boolValue()))) {
        // Send ACK message to the host
        AckMessage* ackMsg = new AckMessage();
        populateWSM(ackMsg);
        ackMsg->setHostIndex(responseMsg->getHostIndex());
        scheduleAt(simTime() + 2 + uniform(1, 2), ackMsg);
    }

    if (findHost()->getIndex() == busIndex) {
        helpersLoad.erase(responseMsg->getHostIndex());
        EV << "Deleted host: " << responseMsg->getHostIndex() << std::endl <<"Host remaining: " << helpersLoad.size() - 1 << std::endl;

        if (helpersLoad.size() == 1) {
            findHost()->getDisplayString().setTagArg("i", 1, "white");
            helpReceived = false;
            sentHelpMessage = false;
            newRandomTime = simTime();

            // Disable the load balancing mode
            loadBalancingState.setState(new Disabled);
        }
    }
}
