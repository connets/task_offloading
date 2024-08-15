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

#include "HelperVehicleInfo.h"

using namespace task_offloading;

HelperVehicleInfo::HelperVehicleInfo() {
    this->index = "auto0";
    this->hostCurrentLoad = 0;
    this->hostCPUFreq = 0;
    this->createdAt = simTime();
    this->dataPartitionId = -1;
    this->vehicleComputationTimer = 0;
}

HelperVehicleInfo::HelperVehicleInfo(const char *index, double load, double freq, inet::L3Address address) {
    this->index = index;
    this->hostCurrentLoad = load;
    this->hostCPUFreq = freq;
    this->createdAt = simTime();
    this->address = address;
    this->dataPartitionId = -1;
    this->vehicleComputationTimer = 0;
}

HelperVehicleInfo::~HelperVehicleInfo() {
    //
}

/**
 * @returns Current index of a vehicle
 */
std::string HelperVehicleInfo::getIndex() {
    return this->index;
}

/**
 * @returns Current load of a vehicle in MB
 */
double HelperVehicleInfo::getCurrentLoad() {
    return this->hostCurrentLoad;
}

/**
 * @returns Current CPU frequency of a vehicle in GHz
 */
double HelperVehicleInfo::getCPUFreq() {
    return this->hostCPUFreq;
}

/**
 * @returns Timestamp corresponding to when the object is created in s
 */
simtime_t HelperVehicleInfo::getCreatedAt() {
    return this->createdAt;
}

/**
 * @returns The L2 address of a vehicle
 */
inet::L3Address HelperVehicleInfo::getAddress() {
    return this->address;
}

/**
 * @returns The angle of a vehicle in degrees
 */
double HelperVehicleInfo::getVehicleAngle() {
    return this->vehicleAngle;
}

/**
 * @returns The current data partition id of the vehicle computation
 */
int HelperVehicleInfo::getDataPartitionId() {
    return this->dataPartitionId;
}

/**
 * @returns The current task cpi -> useful when calculating total compute time
 */
double HelperVehicleInfo::getTaskCpi() {
    return this->taskCpi;
}

/**
 * @returns The speed of a vehicle in m/s
 */
double HelperVehicleInfo::getVehicleSpeed() {
    return this->vehicleSpeed;
}

/**
 * @returns The x position of a vehicle
 */
double HelperVehicleInfo::getVehiclePositionX() {
    return this->vehiclePositionX;
}

/**
 * @returns The y position of a vehicle
 */
double HelperVehicleInfo::getVehiclePositionY() {
    return this->vehiclePositionY;
}

/**
 * @returns The pointer to the current vehicle computation timer
 */
double HelperVehicleInfo::getVehicleComputationTimer() {
    return this->vehicleComputationTimer;
}

/**
 * @returns The total number of responses expected from this vehicle
 */
int HelperVehicleInfo::getResponsesExpected() {
    return this->responsesExpected;
}

/**
 * Set the total value of responses received from vehicle
 *
 * @param newTotal The new total of responses expected from this vehicle
 */
int HelperVehicleInfo::getResponsesReceived() {
    return this->responsesReceived;
}

/**
 * Set the current index of a vehicle
 *
 * @param newIndex The new index of a vehicle
 */
void HelperVehicleInfo::setIndex(std::string index) {
    this->index = index;
}

/**
 * Set the current load of a vehicle
 *
 * @param newLoad The new load of a vehicle
 */
void HelperVehicleInfo::setCurrentLoad(double newLoad) {
    this->hostCurrentLoad = newLoad;
}

/**
 * Set the current CPU frequency of a vehicle
 *
 * @param newFreq The new frequency of a vehicle
 */
void HelperVehicleInfo::setCPUFreq(double newFreq) {
    this->hostCPUFreq = newFreq;
}

/**
 * Set the creation time of the object
 *
 * @param newTime Timestamp when object is created
 */
void HelperVehicleInfo::setCreatedAt(simtime_t newTime) {
    this->createdAt = newTime;
}

/**
 * Set the current L2 address of a vehicle
 *
 * @param newAddress The new address of a vehicle
 */
void HelperVehicleInfo::setAddress(inet::L3Address newAddress) {
    this->address = newAddress;
}

/**
 * Set the current angle of a vehicle in degrees
 *
 * @param newAddress The new angle in degrees of a vehicle
 */
void HelperVehicleInfo::setVehicleAngle(double newAngle) {
    this->vehicleAngle = newAngle;
}

/**
 * Set the current x position of a vehicle
 * @param newAddress The new x position of a vehicle
 */
void HelperVehicleInfo::setVehiclePositionX(double newVehiclePositionX) {
    this->vehiclePositionX=newVehiclePositionX;
}

/**
 * Set the current y position of a vehicle
 * @param newAddress The new y position of a vehicle
 */
void HelperVehicleInfo::setVehiclePositionY(double newVehiclePositionY) {
    this->vehiclePositionY=newVehiclePositionY;
}
/**
 * Set the current data partition id of the vehicle computation
 *
 * @param newPartitionId The new id of a data partition
 */
void HelperVehicleInfo::setDataPartitionId(int newPartitionId) {
    this->dataPartitionId = newPartitionId;
}

/**
 * Set the current cpi of the task
 *
 * @param newCpi The new cpi of the task -> useful when calculating total compute time
 */
void HelperVehicleInfo::setTaskCpi(double newCpi) {
    this->taskCpi = newCpi;
}

/**
 * Set the current speed of a vehicle in m/s
 *
 * @param newSpeed The new current speed of a vehicle
 */
void HelperVehicleInfo::setVehicleSpeed(double newSpeed) {
    this->vehicleSpeed = newSpeed;
}

/**
 * Set the current pointer to vehicle computation timer
 *
 * @param newVehicleComputationTimer The new pointer to the current computation timer message
 */
void HelperVehicleInfo::setVehicleComputationTimer(double newVehicleComputationTimer) {
    this->vehicleComputationTimer = newVehicleComputationTimer;
}

/**
 * Set the total value of responses expected from vehicle
 *
 * @param newTotal The new total of responses expected from this vehicle
 */
void HelperVehicleInfo::setResponsesExpected(int newTotal) {
    this->responsesExpected = newTotal;
}

/**
 * Set the total value of responses received from vehicle
 *
 * @param newTotal The new total of responses expected from this vehicle
 */
void HelperVehicleInfo::setResponsesReceived(int newTotal) {
    this->responsesReceived = newTotal;
}

/**
 * This method calculate the toal computation time of a task
 * considering the current load and the CPU frequency of a vehicle
 *
 * @param CPI Clock per instructions
 * @returns The total computation time for a task
 */
double HelperVehicleInfo::getTotalComputationTime(int CPI, double load) {
    return (CPI * load * (1 / this->hostCPUFreq));
}

/**
 * This method calculate the toal computation time of a task
 * considering the current load and the CPU frequency of a vehicle
 *
 * @param CPI Clock per instructions
 * @returns The total computation time for a task
 */
double HelperVehicleInfo::getTotalComputationTime(int CPI) {
    return this->getTotalComputationTime(CPI, this->hostCurrentLoad);
}

/**
 * This method adds a timer handler into the timers map.
 * Has to be done to trace all timers setted for all vehicles
 * to manage the add or removal of them
 *
 * @param partitionID the data partition id for this timer
 * @param timer the timer to be setted
 */
void HelperVehicleInfo::addTimer(int partitionID, veins::TimerManager::TimerHandle timer) {
    this->timers[partitionID] = timer;
}

/**
 * This method gets the timers handle into the map if found,
 * otherwise it returns -1.
 * Has to be done to trace all timers setted for all vehicles
 * to manage the add or removal of them
 *
 * @param partitionID the data partition id for this timer
 * @returns the timer handler if found, -1 if not found
 */
veins::TimerManager::TimerHandle HelperVehicleInfo::getTimer(int partitionID) {
    auto found = this->timers.find(partitionID);

    if (found != this->timers.end()) {
        return this->timers[partitionID];
    } else {
        return -1;
    }
}

/**
 * This method deletes all the timers setted for all the vehicles
 * Is needed when, for example, the computation ends
 */
void HelperVehicleInfo::clearTimers() {
    this->timers.clear();
}
