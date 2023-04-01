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
    this->hostCurrentLoad = 0;
    this->hostCPUFreq = 0;
    this->createdAt = simTime();
    this->address = 0;
}

HelperVehicleInfo::HelperVehicleInfo(double load, double freq, simtime_t time, veins::LAddress::L2Type address) {
    this->hostCurrentLoad = load;
    this->hostCPUFreq = freq;
    this->createdAt = time;
    this->address = address;
}

HelperVehicleInfo::~HelperVehicleInfo() {
    //
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
 * This method calculate the toal computation time of a task
 * considering the current load and the CPU frequency of a vehicle
 *
 * @param CPI Clock per instructions
 * @returns The total computation time for a task
 */
double HelperVehicleInfo::getTotalComputationTime(int CPI) {
    return (CPI * this->hostCurrentLoad * (1 / this->hostCPUFreq));
}
