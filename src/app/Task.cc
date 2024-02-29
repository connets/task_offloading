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

#include "Task.h"

using namespace task_offloading;

Define_Module(task_offloading::Task);

void Task::initialize() {
    // Initialize operations
    // Set all the parameters from NED file
    this->id = par("id").intValue();
    this->totalData = par("totalData").doubleValue();
    this->minimumLoadRequested = par("minimumLoadRequested").doubleValue();
    this->helpReceivedCounter = par("helpReceivedCounter").intValue();
    this->dataPartitionId = par("dataPartitionId").intValue();
    this->loadBalancingId = par("loadBalancingId").intValue();
    this->availableReceivedCounter = par("availableReceivedCounter").intValue();
    this->responseReceivedCounter = par("responseReceivedCounter").intValue();
    this->computingDensity = par("computingDensity").intValue();
}

void Task::finish() {
    // Finish operation
}

/**
 * Getters section
 */

/**
 * Get the current id of the task
 *
 * @return int
 */
int Task::getId() {
    return this->id;
}

/**
 * Get the total computation data
 *
 * @return double
 */
double Task::getTotalData() {
    return this->totalData;
}

/**
 * Get the counter of how many times the BUS received help
 *
 * @return int
 */
int Task::getHelpReceivedCounter() {
    return this->helpReceivedCounter;
}


/**
 * Get the current data partition id
 *
 * @return int
 */
long Task::getDataPartitionId() {
    return this->dataPartitionId;
}

/**
 * Get the current load balancing id
 *
 * @return int
 */
int Task::getLoadBalancingId() {
    return this->loadBalancingId;
}

/**
 * Get the counter of vehicles available
 *
 * @return int
 */
int Task::getAvailableReceivedCounter() {
    return this->availableReceivedCounter;
}

/**
 * Get the counter of response received from vehicles
 *
 * @return int
 */
int Task::getResponseReceivedCounter() {
    return this->responseReceivedCounter;
}

/**
 * Get the minimum load requested accepted by bus
 *
 * @return double
 */
double Task::getMinimumLoadRequested() {
    return this->minimumLoadRequested;
}

/**
 * Get the CPI for the task, it's a constant that represent the difficulty of it
 *
 * @return int
 */
int Task::getComputingDensity() {
    return this->computingDensity;
}

/**
 * Setters section
 */

/**
 * Set the current task id
 *
 * @return void
 */
void Task::setId(int newId) {
    this->id = newId;
}

/**
 * Set the current computation load
 *
 * @return void
 */
void Task::setTotalData(double newData) {
    this->totalData = newData;
}

/**
 * Increment the help received counter by one
 *
 * @return void
 */
void Task::setHelpReceivedCounter(int newCounter) {
    this->helpReceivedCounter = newCounter;
}

/**
 * Set the new data partition id
 *
 * @return void
 */
void Task::setDataPartitionId(long newDataPartitionId) {
    this->dataPartitionId = newDataPartitionId;
}

/**
 * Set the new load balancing id
 *
 * @return void
 */
void Task::setLoadBalancingId(int newLoadBalancingId) {
    this->loadBalancingId = newLoadBalancingId;
}

/**
 * Set the new vehicles available counter
 *
 * @return void
 */
void Task::setAvailableReceivedCounter(int newCounter) {
    this->availableReceivedCounter = newCounter;
}

/**
 * Set the new responses received counter
 *
 * @return void
 */
void Task::setResponseReceivedCounter(int newCounter) {
    this->responseReceivedCounter = newCounter;
}

/**
 * Set the minimum load requested accepted by bus
 *
 * @return void
 */
void Task::setMinimumLoadRequested(double newMinimumLoad) {
    this->minimumLoadRequested = newMinimumLoad;
}

/**
 * Set the CPI for the task, it's a constant that represent the difficulty of it
 *
 * @return void
 */
void Task::setComputingDensity(int newCpi) {
    this->computingDensity = newCpi;
}
