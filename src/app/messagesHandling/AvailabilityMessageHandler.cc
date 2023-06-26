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

   double aRt =par("availabilityRangeTime").doubleValue();

   // Calculate time for computation
   double CPI = tasks[0].getComputingDensity();
   double CR = availabilityMessage->getCpuFreq();

   //Calculate bitrate conversion from megabit to megabyte
   double bitRate = getModuleByPath(".^.nic.mac1609_4")->par("bitrate").intValue() / 8.0;

   //Calculate the available load for the car
   double localData = (availabilityMessage->getAvailableLoad());

   double IO = par("IOratio").doubleValue();

   double transferTime = localData/bitRate;
   double transferTimeRes =(localData*IO)/bitRate;
   double timeToCompute = CPI * localData * (1 / CR);

   if(aRt==-1.0) {
        //Generate the time that is used to check whether a car will be in the bus range in those next seconds
        aRt = transferTime + timeToCompute + transferTimeRes;

    }
    //car position at the current time
    double cx = helpers[availabilityMessage->getHostID()].getVehiclePositionX();
    double cy = helpers[availabilityMessage->getHostID()].getVehiclePositionY();
    //bus position at the current time
    veins::TraCIMobility* mobilityMod = check_and_cast<veins::TraCIMobility*>(getModuleByPath("^.veinsmobility"));
    double bx = mobilityMod->getPositionAt(simTime()).x;
    double by = mobilityMod->getPositionAt(simTime()).y;

    //future positions of car (next aRt seconds)
//    double nextCx = (helpers[availabilityMessage->getHostID()].getVehicleSpeed()*aRt*std::cos(helpers[availabilityMessage->getHostID()].getVehicleAngle())) + cx;
//    double nextCy = (helpers[availabilityMessage->getHostID()].getVehicleSpeed()*aRt*std::sin(helpers[availabilityMessage->getHostID()].getVehicleAngle())) + cy;
    //future positions of car (next aRt seconds)
    double nextBx = (traciVehicle->getSpeed()*aRt*std::cos(traciVehicle->getAngle())) + bx;
    double nextBy = (traciVehicle->getSpeed()*aRt*std::sin(traciVehicle->getAngle())) + by;

    //radius for both the car and bus reach in their next 15 seconds
//    double radiusCar = sqrt(pow((nextCx - cx),2.0) + pow((nextCy - cy),2.0));
    double radiusBus = sqrt(pow((nextBx - bx),2.0) + pow((nextBy - by),2.0));

    //check if both the bus and the car will be in each other reach in the next 15 seconds
    //((x-center_x)^2 + (y-center_y)^2 < radius^2)
    if(!(pow(cx-nextBx,2.0) + pow(cy-nextBy,2.0) < pow(radiusBus,2.0))) {
        //remove vehicle availability due to projected long distance
        helpers.erase(availabilityMessage->getHostID());
        helpersOrderedList.remove(availabilityMessage->getHostID());
    }

    // Check the bus state
    int currentBusState = busState.getCurrentState();

    if (currentBusState == 1) {
        // Color the bus that received help
        findHost()->getDisplayString().setTagArg("i", 1, "green");
        std::string currentHostIndex = availabilityMessage->getIndex() + std::to_string(availabilityMessage->getHostID());
        double currentLoad = availabilityMessage->getAvailableLoad();
        double CPUFreq = availabilityMessage->getCpuFreq();
        veins::LAddress::L2Type address = availabilityMessage->getSenderAddress();
        double vehicleAngle = availabilityMessage->getVehicleAngle();
        double vehicleSpeed = availabilityMessage->getVehicleSpeed();
        double vehiclePosX = availabilityMessage->getVehiclePositionX();
        double vehiclePosY = availabilityMessage->getVehiclePositionY();

        helpers[availabilityMessage->getHostID()] = HelperVehicleInfo(currentHostIndex, currentLoad, CPUFreq, address);
        helpers[availabilityMessage->getHostID()].setVehicleAngle(vehicleAngle);
        helpers[availabilityMessage->getHostID()].setVehicleSpeed(vehicleSpeed);
        helpers[availabilityMessage->getHostID()].setTaskCpi(tasks[0].getComputingDensity());
        helpers[availabilityMessage->getHostID()].setVehiclePositionX(vehiclePosX);
        helpers[availabilityMessage->getHostID()].setVehiclePositionY(vehiclePosY);

        int previousAvailability = tasks[0].getAvailableReceivedCounter();
        previousAvailability++;
        tasks[0].setAvailableReceivedCounter(previousAvailability);
    }
}
