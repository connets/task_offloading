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

using namespace task_offloading;

void Worker::handleHelpMessage(HelpMessage* helpMessage)
{
    // I've received an help request from bus

    // Emit the help message received
    emit(stopHelp, simTime());

    // First check if I met requirements for the bus
    double minimumLoadRequested = helpMessage->getMinimumLoadRequested();

    // Check my current load
    double currentVehicleLoad = par("randomVehicleFreeLoadPercentage").doubleValue() * par("commonVehicleLoad").doubleValue();
    availableLoad = availableLoad - currentVehicleLoad;

    // Emit the signal for my current load
    emit(availableMessageLoad, currentVehicleLoad);

    // Check my current CPU freq
    double CPUFreq = par("randomVehicleCpuFreq").doubleValue();

    // Set my CPU freq
    cpuFreq = CPUFreq;

    // Save the bus address
    busAddress = helpMessage->getSenderAddress();

    // If I met requirements send an available message
    if (currentVehicleLoad >= minimumLoadRequested) {
        //start task availability timer
        setTaskAvailabilityTimer(helpMessage->getId(),helpMessage->getTaskSize());

        // Color the vehicle icon in blue
        findHost()->getDisplayString().setTagArg("i", 1, "blue");

        // Prepare the availability message
        AvailabilityMessage* available = new AvailabilityMessage();

        // Populate the message
        populateWSM(available);
        available->setHostID(findHost()->getIndex());
        available->setIndex(findHost()->getName());
        available->setAvailableLoad(currentVehicleLoad);
        available->setCpuFreq(cpuFreq);
        available->setVehicleAngle(traciVehicle->getAngle());
        available->setVehicleSpeed(traciVehicle->getSpeed());
        veins::TraCIMobility* mobilityMod = check_and_cast<veins::TraCIMobility*>(getModuleByPath("^.veinsmobility"));
        double cx = mobilityMod->getPositionAt(simTime()).x;
        double cy = mobilityMod->getPositionAt(simTime()).y;
        available->setVehiclePositionX(cx);
        available->setVehiclePositionY(cy);
        available->setSenderAddress(mac->getMACAddress());
        available->setRecipientAddress(helpMessage->getSenderAddress());

        // Schedule the ok message
        scheduleAt(simTime() + par("vehicleAvailabilityMessageTime").doubleValue(), available);
    }
}
