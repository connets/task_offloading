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

#include "BusVehicleInfo.h"

using namespace task_offloading;

BusVehicleInfo::BusVehicleInfo() {
    this->index = "bus0";
    this->createdAt = simTime();
}

BusVehicleInfo::BusVehicleInfo(std::string index) {
    this->index = index;
    this->createdAt = simTime();
}

BusVehicleInfo::~BusVehicleInfo() {
    //
}

/**
 * @returns Current index of a vehicle
 */
std::string BusVehicleInfo::getIndex() {
    return this->index;
}

/**
 * @returns Timestamp corresponding to when the object is created in s
 */
simtime_t BusVehicleInfo::getCreatedAt() {
    return this->createdAt;
}

/**
 * @returns The angle of a vehicle in degrees
 */
double BusVehicleInfo::getVehicleAngle() {
    return this->vehicleAngle;
}

/**
 * @returns The position of the named vehicle within the last step [X,m].
 */
double BusVehicleInfo::getVehiclePositionX(){
    return this->vehiclePositionX;
}

/**
 * @returns The position of the named vehicle within the last step [m,Y].
 */
double BusVehicleInfo::getVehiclePositionY(){
    return this->vehiclePositionY;
}

/**
 * @returns The speed of a vehicle in m/s
 */
double BusVehicleInfo::getVehicleSpeed() {
    return this->vehicleSpeed;
}

/**
 * Set the current index of a vehicle
 *
 * @param newIndex The new index of a vehicle
 */
void BusVehicleInfo::setIndex(std::string index) {
    this->index = index;
}

/**
 * Set the creation time of the object
 *
 * @param newTime Timestamp when object is created
 */
void BusVehicleInfo::setCreatedAt(simtime_t newTime) {
    this->createdAt = newTime;
}

/**
 * Set the current angle of a vehicle in degrees
 *
 * @param newAngle The new angle in degrees of a vehicle
 */
void BusVehicleInfo::setVehicleAngle(double newAngle) {
    this->vehicleAngle = newAngle;
}

/** Set the position of the named vehicle within the last step [X,m].
 *
 *  @param newPosition The new position of the vehicle
 */
void BusVehicleInfo::setVehiclePositionX(double newPositionX) {
    this->vehiclePositionX = newPositionX;
}

/** Set the position of the named vehicle within the last step [m,Y].
 *
 *  @param newPosition The new position of the vehicle
 */
void BusVehicleInfo::setVehiclePositionY(double newPositionY) {
    this->vehiclePositionY = newPositionY;
}

/**
 * Set the current speed of a vehicle in m/s
 *
 * @param newSpeed The new current speed of a vehicle
 */
void BusVehicleInfo::setVehicleSpeed(double newSpeed) {
    this->vehicleSpeed = newSpeed;
}
