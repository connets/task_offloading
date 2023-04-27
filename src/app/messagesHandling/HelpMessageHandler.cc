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

void VeinsApp::handleHelpMessage(HelpMessage* helpMsg)
{
    // Check that the help request come from the bus
    if (helpMsg->getVehicleIndex() == busIndex) {
        // Color the host that received the help message
        findHost()->getDisplayString().setTagArg("i", 1, "yellow");

        // Set for every single host a random value for current load and cpu freq
        double randomVehicleLoad = par("randomVehicleLoadActual").doubleValue();
        double commonVehicleLoad = par("commonVehicleLoad").doubleValue();

        // Get the CPU freq and assign to host variable
        double cpuFreq = par("randomCpuVehicleFreq").doubleValue();
        hostCpuFreq = cpuFreq;

        // Calculate real actual load
        // Actual load = common load (MB) * random actual load (% between 0 and 1)
        double actualLoad = commonVehicleLoad * randomVehicleLoad;
        double minimumLoadRequested = helpMsg->getMinimumLoadRequested();

        // If the host is available send an ok message after
        // some time with ID and the computation load available
        if (actualLoad >= minimumLoadRequested) {
            // Prepare the message
            OkMessage* okMsg = new OkMessage();
            populateWSM(okMsg);
            okMsg->setSenderAddress(myId);
            okMsg->setHostID(findHost()->getIndex());
            okMsg->setAvailableLoad(actualLoad);
            okMsg->setCpuFreq(cpuFreq);
            okMsg->setIndex(findHost()->getName());
            okMsg->setVehicleAngle(traciVehicle->getAngle());

            // Emit the signal of the ok message load
            emit(okMessageLoad, actualLoad);

            // Schedule the ok message
            scheduleAt(simTime() + par("vehicleOkMessageTime").doubleValue(), okMsg);
        } else if (findHost()->getIndex() != busIndex && actualLoad < minimumLoadRequested) {
            findHost()->getDisplayString().setTagArg("i", 1, "white");
        }
    }
}
