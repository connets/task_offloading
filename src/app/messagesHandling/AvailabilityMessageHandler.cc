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

void TaskGenerator::handleAvailabilityMessage(AvailabilityMessage* availabilityMessage)
{
    // Check the bus state
    int currentBusState = busState.getCurrentState();

    if (findHost()->getIndex() == busIndex && currentBusState == 1) {
        // Color the bus that received help
        findHost()->getDisplayString().setTagArg("i", 1, "green");
        std::string currentHostIndex = availabilityMessage->getIndex() + std::to_string(availabilityMessage->getHostID());
        double currentLoad = availabilityMessage->getAvailableLoad();
        double CPUFreq = availabilityMessage->getCpuFreq();
        veins::LAddress::L2Type address = availabilityMessage->getSenderAddress();
        double vehicleAngle = availabilityMessage->getVehicleAngle();

        helpers[availabilityMessage->getHostID()] = HelperVehicleInfo(currentHostIndex, currentLoad, CPUFreq, address);
        helpers[availabilityMessage->getHostID()].setVehicleAngle(vehicleAngle);

        int previousAvailability = tasks[0].getAvailableReceivedCounter();
        previousAvailability++;
        tasks[0].setAvailableReceivedCounter(previousAvailability);
    }
}
