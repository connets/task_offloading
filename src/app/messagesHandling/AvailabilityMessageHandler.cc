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

using namespace task_offloading;

void TaskGenerator::handleOkMessage(OkMessage* okMsg)
{
    if (findHost()->getIndex() == busIndex) {
        // Store the helper load and CPU freq only if the load has minimum requirements
        if (okMsg->getAvailableLoad() >= par("minimumVehicleLoadActual").doubleValue()) {
            // Color the bus that received help
            findHost()->getDisplayString().setTagArg("i", 1, "green");
            std::string currentHostIndex = okMsg->getIndex() + std::to_string(okMsg->getHostID());
            double currentLoad = okMsg->getAvailableLoad();
            double CPUFreq = okMsg->getCpuFreq();
            veins::LAddress::L2Type address = okMsg->getSenderAddress();
            double vehicleAngle = okMsg->getVehicleAngle();

            helpers[okMsg->getHostID()] = HelperVehicleInfo(currentHostIndex, currentLoad, CPUFreq, address);
            helpers[okMsg->getHostID()].setVehicleAngle(vehicleAngle);

            okReceived++;
        }
    }
}
