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

using namespace task_offloading;

void VeinsApp::handleOkMessage(OkMessage* okMsg)
{
    if (findHost()->getIndex() == busIndex) {
        // Store the helper load and CPU freq only if the load has minimum requirements
        if (okMsg->getAvailableLoad() >= par("minimumVehicleLoadActual").doubleValue()) {
            // Color the bus that received help
            findHost()->getDisplayString().setTagArg("i", 1, "green");
            std::string currentHostIndex = okMsg->getIndex() + std::to_string(okMsg->getHostID());

            helpers[okMsg->getHostID()] = HelperVehicleInfo();
            helpers[okMsg->getHostID()].setCurrentLoad(okMsg->getAvailableLoad());
            helpers[okMsg->getHostID()].setCPUFreq(okMsg->getCpuFreq());
            helpers[okMsg->getHostID()].setAddress(okMsg->getSenderAddress());
            helpers[okMsg->getHostID()].setIndex(currentHostIndex);

            okReceived++;
        }
    }
}
