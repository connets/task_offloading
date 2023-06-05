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
    this->address = 0;
    this->dataPartitionId = -1;
}

HelperVehicleInfo::HelperVehicleInfo(std::string index, double load, double freq, veins::LAddress::L2Type address) {
    this->index = index;
    this->hostCurrentLoad = load;
    this->hostCPUFreq = freq;
    this->createdAt = simTime();
    this->address = address;
    this->dataPartitionId = -1;
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
veins::LAddress::L2Type HelperVehicleInfo::getAddress() {
    return this->address;
}

/**
 * @returns The angle of a vehicle in degrees
 */
double HelperVehicleInfo::getVehicleAngle() {
    return this->vehicleAngle;
}

/**
 * @returns The position of the named vehicle within the last step [X,m].
 */
double HelperVehicleInfo::getVehiclePositionX(){
    return this->vehiclePositionX;
}

/**
 * @returns The position of the named vehicle within the last step [m,Y].
 */
double HelperVehicleInfo::getVehiclePositionY(){
    return this->vehiclePositionY;
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
int HelperVehicleInfo::getTaskCpi() {
    return this->taskCpi;
}

/**
 * @returns The speed of a vehicle in m/s
 */
double HelperVehicleInfo::getVehicleSpeed() {
    return this->vehicleSpeed;
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
void HelperVehicleInfo::setAddress(veins::LAddress::L2Type newAddress) {
    this->address = newAddress;
}

/**
 * Set the current angle of a vehicle in degrees
 *
 * @param newAngle The new angle in degrees of a vehicle
 */
void HelperVehicleInfo::setVehicleAngle(double newAngle) {
    this->vehicleAngle = newAngle;
}

/** Set the position of the named vehicle within the last step [X,m].
 *
 *  @param newPosition The new position of the vehicle
 */
void HelperVehicleInfo::setVehiclePositionX(double newPositionX) {
    this->vehiclePositionX = newPositionX;
}

/** Set the position of the named vehicle within the last step [m,Y].
 *
 *  @param newPosition The new position of the vehicle
 */
void HelperVehicleInfo::setVehiclePositionY(double newPositionY) {
    this->vehiclePositionY = newPositionY;
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
void HelperVehicleInfo::setTaskCpi(int newCpi) {
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
 * This method calculate the toal computation time of a task
 * considering the current load and the CPU frequency of a vehicle
 *
 * @param CPI Clock per instructions
 * @returns The total computation time for a task
 */
double HelperVehicleInfo::getTotalComputationTime(int CPI) {
    return (CPI * this->hostCurrentLoad * (1 / this->hostCPUFreq));
}
