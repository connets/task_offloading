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
    delete this;
}

double HelperVehicleInfo::getCurrentLoad() {
    return this->hostCurrentLoad;
}

double HelperVehicleInfo::getCPUFreq() {
    return this->hostCPUFreq;
}

simtime_t HelperVehicleInfo::getCreatedAt() {
    return this->createdAt;
}

veins::LAddress::L2Type HelperVehicleInfo::getAddress() {
    return this->address;
}

void HelperVehicleInfo::setCurrentLoad(double newLoad) {
    this->hostCurrentLoad = newLoad;
}

void HelperVehicleInfo::setCPUFreq(double newFreq) {
    this->hostCPUFreq = newFreq;
}

void HelperVehicleInfo::setCreatedAt(simtime_t newTime) {
    this->createdAt = newTime;
}

void HelperVehicleInfo::setAddress(veins::LAddress::L2Type newAddress) {
    this->address = newAddress;
}

double HelperVehicleInfo::getTotalComputationTime(int CPI) {
    return (CPI * this->hostCurrentLoad * (1 / this->hostCPUFreq));
}
