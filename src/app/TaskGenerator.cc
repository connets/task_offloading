//
// Copyright (C) 2016 David Eckhoff <david.eckhoff@fau.de>
//
// Documentation for these modules is at http://veins.car2x.org/
//
// SPDX-License-Identifier: GPL-2.0-or-later
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#include "TaskGenerator.h"

#include "app/messages/HelpMessage_m.h"
#include "app/messages/AvailabilityMessage_m.h"
#include "app/messages/BeaconMessage_m.h"
#include "app/messages/AckMessage_m.h"
#include "app/messages/DataMessage_m.h"
#include "app/messages/ResponseMessage_m.h"
#include "app/messages/AckTimerMessage_m.h"
#include "app/messages/LoadBalanceTimerMessage_m.h"
#include "app/messages/ComputationTimerMessage_m.h"
#include "app/messages/UpdateAvailabilityMessage_m.h"
#include "app/messages/TimerFirstHelpMessage_m.h"

using namespace task_offloading;
using namespace inet;

Define_Module(task_offloading::TaskGenerator);

void TaskGenerator::initialize(int stage)
{
    veins::VeinsInetApplicationBase::initialize(stage);

    if (stage == 0) {
        // Initializing members and pointers of your application goes here
        lastDroveAt = simTime();

        // BUS SECTION
        // Set the BUS index
        generatorIndex = getParentModule()->getIndex();

        // Initialize the BUS state
        busState = BusContext(new Help);

        // Initialize the load balancing algorithm
        loadBalancingAlgorithm = check_and_cast<BaseSorting*>(findModuleByPath("task_offloading.loadBalancingAlgorithm"));

        // Initialize the number of total responses I expect
        totalReponsesExpected = 0;

        // Registering all signals
        startTask = registerSignal("task_started");
        stopTask = registerSignal("task_stopped");
        startBalance = registerSignal("start_balance_loading");
        stopBalance = registerSignal("stop_balance_loading");
        startHelp = registerSignal("start_bus_help_rq");
        startDataMessages = registerSignal("start_sending_data");
        stopBeaconMessages = registerSignal("stopBeaconMessages");
        stopResponseMessages = registerSignal("stop_getting_response");
    }
}

void TaskGenerator::handleStartOperation(inet::LifecycleOperation* doneCallback)
{
    veins::VeinsInetApplicationBase::handleStartOperation(doneCallback);
    // Start the timer for the first help message
    double time = par("randomTimeFirstHelpMessage").doubleValue();

    auto callback = [this]() {
        vehicleHandler();
    };

    timerManager.create(veins::TimerSpecification(callback).oneshotIn(time));
}

void TaskGenerator::handleStopOperation(inet::LifecycleOperation* doneCallback)
{
    veins::VeinsInetApplicationBase::handleStopOperation(doneCallback);
    // Close the socket
    socket.close();
}

void TaskGenerator::finish()
{
    veins::VeinsInetApplicationBase::finish();
}

void TaskGenerator::processPacket(std::shared_ptr<inet::Packet> pk)
{
    /************************************************************************
      Your application has received a data message from another car or RSU
    ************************************************************************/
    try {
        if (pk->hasData<BasePacket>()) {
            auto packet = pk->peekData<BasePacket>();
            BasePacket* data = packet->dup();

            // SECTION - When the bus receives the ok messages
            if (data->getType() == AVAILABILITY) {
                auto dataFromPacket = pk->peekData<AvailabilityMessage>();
                AvailabilityMessage* availabilityMessage = dataFromPacket->dup();
                handleAvailabilityMessage(availabilityMessage);
            }

            // SECTION - When the bus receive the response message
            if (data->getType() == RESPONSE) {
                auto dataFromPacket = pk->peekData<ResponseMessage>();
                ResponseMessage* responseMessage = dataFromPacket->dup();

                // Check if the response message is for me
                if (responseMessage->getGeneratorIndex() == getParentModule()->getIndex()) {
                    handleResponseMessage(responseMessage);
                }
            }

            if (data->getType() == BEACON) {
                emit(stopBeaconMessages, simTime());
            }
        }
    } catch (cException e) {
        EV << "Package type not expected" << std::endl;
    }
}

void TaskGenerator::balanceLoad()
{
    // We have to do some work -> load balance!

    // But first change the bus state to load balancing
    busState.setState(new LoadBalancing);

    // Then order the map of helpers by sorting them with the chosen sorting algorithm
    helpersOrderedList = loadBalancingAlgorithm->sort(helpers);

    // Store the data into a local variable so can be used
    double localData = tasks[0]->getTotalData();

    // Emit the start of load balancing
    emit(startBalance, simTime());

    // For each vehicle prepare the data message and send
    for (auto const &i: helpersOrderedList) {
        // Check for the vehicle availability
        // If it is greater than the maximum size of and UDP message (65535B)
        // then divide the packet into more data fragments
        int n_fragments = 1;

        if (B(helpers[i].getCurrentLoad()) >= B(65535)) {
            n_fragments = ((int)(helpers[i].getCurrentLoad() / 65535)) + 1;
        }

        // Get the current data partition id
        int currentPartitionId = tasks[0]->getDataPartitionId();

        // Get the vehicle availablity
        auto currentVehicleAvailability = helpers[i].getCurrentLoad();

        // Get the maximum value for udp message
        auto UDPMaxVal = par("UDPMaxLength").doubleValue();

        // Set the responses I expect from this vehicle
        int responsesExpectedFromVehicle = n_fragments;

        helpers[i].setResponsesExpected(n_fragments);

        for (int j = 0; j < n_fragments; ++j) {
            // Check if there's data to load
            if (localData > 0) {
                // Prepare the data message
                // DataMessage* dataMessage = new DataMessage();
                auto dataMessage = makeShared<DataMessage>();

                // Calculate the chunk length between vehicle availability and max UDP packet size taken from NED
                auto chunkLength = std::min((currentVehicleAvailability - UDPMaxVal), UDPMaxVal);

                // If chunk length is > 0 then set it as data message chunk length
                // otherwise set the chunk length as the remaining vehicle availability
                if (chunkLength > 0) {
                    // Set the byte length
                    dataMessage->setChunkLength(B(chunkLength));

                    // Set the message load to process
                    dataMessage->setLoadToProcess(chunkLength);

                    // Update variables
                    currentVehicleAvailability = currentVehicleAvailability - UDPMaxVal;

                    // Check for local data value
                    if ((localData - UDPMaxVal) > 0) {
                        localData = localData - UDPMaxVal;
                    } else {
                        localData = 0;
                    }
                } else {
                    // Set the byte length
                    dataMessage->setChunkLength(B(currentVehicleAvailability));

                    // Set the message load to process
                    dataMessage->setLoadToProcess(currentVehicleAvailability);

                    // Update variables
                    currentVehicleAvailability = 0;

                    // Check for local data value
                    if ((localData - currentVehicleAvailability) > 0) {
                        localData = localData - currentVehicleAvailability;
                    } else {
                        localData = 0;
                    }
                }

                // Populate the other fields
                // L3Address generator = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
                // dataMessage->setSenderAddress(generator);
                dataMessage->setHostIndex(i);
                dataMessage->setTaskId(tasks[0]->getId());
                dataMessage->setTaskSize(tasks[0]->getTotalData());
                dataMessage->setPartitionId(tasks[0]->getDataPartitionId());
                dataMessage->setLoadBalancingId(tasks[0]->getLoadBalancingId());
                dataMessage->setCpi(tasks[0]->getComputingDensity());
                dataMessage->setResponsesExpected(responsesExpectedFromVehicle);

                // Calculate time for timer
                double CPI = tasks[0]->getComputingDensity();
                double timeToCompute = helpers[i].getTotalComputationTime(CPI, currentVehicleAvailability);

                dataMessage->setComputationTime(timeToCompute);

                // Save into the helper the data partition ID
                helpers[i].setDataPartitionId(currentPartitionId);

                // Create timer computation message for each host if auto ACKs are disabled
                if (par("useAcks").boolValue() == false) {
                    // Calculate time to file transmission
                    // Calculate bitrate conversion from megabit to megabyte
                    double bitRate = findModuleByPath(".^.wlan[*]")->par("bitrate").doubleValue() / 8.0;
                    double transferTime = localData / bitRate;

                    // Save the computation timer into helpers map
                    helpers[i].setVehicleComputationTimer(timeToCompute + transferTime + par("dataComputationThreshold").doubleValue());

                    double time = (timeToCompute + transferTime + par("dataComputationThreshold").doubleValue());

                    // The & inside the square brackets tells to capture all local variable
                    // by value
                    auto callback = [=]() {
                        sendAgainData(dataMessage->dup());
                    };
                    // Create the timer and save it to the timers map into helpers object
                    veins::TimerManager::TimerHandle timer = timerManager.create(veins::TimerSpecification(callback).oneshotIn(time));
                    helpers[i].addTimer(currentPartitionId, timer);
                }

                // Schedule the data packet
                auto dataPacket = createPacket("data_message");
                dataPacket->insertAtBack(dataMessage);
                sendPacket(std::move(dataPacket));
            }
            // Increment data partition ID
            currentPartitionId++;
            tasks[0]->setDataPartitionId(currentPartitionId);

            // Increment the total number of responses I expect from vehicles
            totalReponsesExpected++;
        }
    }

    // Change the bus state to data transfer
    busState.setState(new DataTransfer);

    // Emit the stop of load balancing
    emit(stopBalance, simTime());
}

void TaskGenerator::vehicleHandler()
{
    if (tasks[0] == nullptr) {
        // Create task module
        cModuleType *moduleType = cModuleType::get("task_offloading.app.Task");
        cModule *module = moduleType->create("task", findModuleByPath("<root>"));

        // Initialize all parameters
        module->par("id") = 0;
        module->par("totalData") = par("computationLoad").doubleValue();
        module->par("minimumLoadRequested") = par("minimumVehicleLoadRequested").doubleValue();
        module->par("computingDensity") = 3;
        module->finalizeParameters();

        // Create internals and start module
        module->buildInside();
        module->callInitialize();
        module->scheduleStart(simTime());

        Task *task = check_and_cast<Task*>(module);
        tasks[0] = std::move(task);
    }

    // Get the timer for the first help message
    // bool timerForFirstHelpMessage = simTime() > par("randomTimeFirstHelpMessage").doubleValue();
    // Check the bus state
    int currentBusState = busState.getCurrentState();
    // Check if there's more data
    bool moreDataToLoad = tasks[0]->getTotalData() > 0;

    // Check if we reach the time of the first help message
    if ((currentBusState == 0) && moreDataToLoad && helpers.size() == 0) {
        // Color the bus icon in red
        getParentModule()->getDisplayString().setTagArg("i", 1, "red");

        // Prepare the help message
        auto helpMessage = makeShared<HelpMessage>();

        // Populate the message
        helpMessage->setId(tasks[0]->getId());
        helpMessage->setGeneratorIndex(generatorIndex);
        helpMessage->setCpi(tasks[0]->getComputingDensity());
        helpMessage->setTaskSize(tasks[0]->getTotalData());
        helpMessage->setMinimumLoadRequested(tasks[0]->getMinimumLoadRequested());
        helpMessage->setChunkLength(B(200));

        // Emit signal for start help message
        emit(startHelp, simTime());

        // Send the packet in broadcast
        auto packet = createPacket("help_message");
        packet->insertAtBack(helpMessage);
        sendPacket(std::move(packet));

        if (tasks[0]->getHelpReceivedCounter() == 0) {
            emit(startTask, simTime());
        }

        // Change the load balancing state
        busState.setState(new LoadBalancing);

        // Increment the counter for help messages
        int helpCounter = tasks[0]->getHelpReceivedCounter();
        helpCounter++;
        tasks[0]->setHelpReceivedCounter(helpCounter);

        double time = par("busWaitingTimeForAvailability").doubleValue();

        auto callback = [this]() {
            // If I'm the bus and I've received help then load balance
            // otherwise back to help messages
            if (helpers.size() > 0 && busState.getCurrentState() == 1) {
                // Call to load balancing function
                balanceLoad();
            } else if (helpers.size() == 0) {
                // Disable load balancing
                busState.setState(new Help);
                // Restart the vehicle handler function
                vehicleHandler();
            }
        };

        // Schedule the message -> simTime + availability msgs threshold
        timerManager.create(veins::TimerSpecification(callback).oneshotIn(time));
    } else if (tasks[0]->getTotalData() <= 0) {
        // Color the bus in white when computation ends
        getParentModule()->getDisplayString().setTagArg("i", 1, "white");
    }
}

void TaskGenerator::handleAvailabilityMessage(AvailabilityMessage* availabilityMessage)
{

    double aRt = par("availabilityRangeTime").doubleValue();

    // Calculate time for computation
    double CPI = tasks[0]->getComputingDensity();
    double CR = availabilityMessage->getCpuFreq();

    // Calculate bitrate conversion from megabit to megabyte
    double bitRate = findModuleByPath(".^.wlan[*]")->par("bitrate").doubleValue() / 8.0;

    // Calculate the available load for the car
    double localData = (availabilityMessage->getAvailableLoad());

    double IO = par("IOratio").doubleValue();

    double transferTime = localData/bitRate;
    double transferTimeRes =(localData*IO)/bitRate;
    double timeToCompute = CPI * localData * (1 / CR);

    if(aRt==-1.0) {
        // Generate the time that is used to check whether a car will be in the bus range in those next seconds
        aRt = (transferTime + timeToCompute + transferTimeRes)*par("retryFactorTime").doubleValue();
    }

    // Car position at the current time
    double cx = helpers[availabilityMessage->getHostID()].getVehiclePositionX();
    double cy = helpers[availabilityMessage->getHostID()].getVehiclePositionY();
    // Bus position at the current time
    // veins::TraCIMobility* mobilityMod = check_and_cast<veins::TraCIMobility*>(findModuleByPath("^.veinsmobility"));
    double bx = mobility->getCurrentPosition().x;
    double by = mobility->getCurrentPosition().y;

    // Future positions of car (next aRt seconds)
    double nextCx = (helpers[availabilityMessage->getHostID()].getVehicleSpeed()*aRt*std::cos(helpers[availabilityMessage->getHostID()].getVehicleAngle())) + cx;
    double nextCy = (helpers[availabilityMessage->getHostID()].getVehicleSpeed()*aRt*std::sin(helpers[availabilityMessage->getHostID()].getVehicleAngle())) + cy;
    // Future positions of car (next aRt seconds)
    double nextBx = (traciVehicle->getSpeed()*aRt*std::cos(traciVehicle->getAngle())) + bx;
    double nextBy = (traciVehicle->getSpeed()*aRt*std::sin(traciVehicle->getAngle())) + by;

    // Radius for both the car and bus reach in their next aRt seconds
    // double radiusCar = sqrt(pow((nextCx - cx),2.0) + pow((nextCy - cy),2.0));
    double radiusBus = sqrt(pow((nextBx - bx),2.0) + pow((nextBy - by),2.0));

    // Check if both the bus and the car will be in each other reach in the next aRt seconds
    // ((x-center_x)^2 + (y-center_y)^2 < radius^2)
    if(!(pow(nextCx-nextBx,2.0) + pow(nextCy-nextBy,2.0) < pow(radiusBus,2.0))) {
        // remove vehicle availability due to projected long distance
        helpers.erase(availabilityMessage->getHostID());
        helpersOrderedList.remove(availabilityMessage->getHostID());
    }

    // Check the bus state
    int currentBusState = busState.getCurrentState();

    if (currentBusState == 1) {
        // Color the bus that received help
        getParentModule()->getDisplayString().setTagArg("i", 1, "green");
        std::string currentHostIndex = availabilityMessage->getIndex() + std::to_string(availabilityMessage->getHostID());
        double currentLoad = availabilityMessage->getAvailableLoad();
        double CPUFreq = availabilityMessage->getCpuFreq();
        L3Address address = availabilityMessage->getSenderAddress();
        double vehicleAngle = availabilityMessage->getVehicleAngle();
        double vehicleSpeed = availabilityMessage->getVehicleSpeed();
        double vehiclePosX = availabilityMessage->getVehiclePositionX();
        double vehiclePosY = availabilityMessage->getVehiclePositionY();

        helpers[availabilityMessage->getHostID()] = HelperVehicleInfo(currentHostIndex, currentLoad, CPUFreq, address);
        helpers[availabilityMessage->getHostID()].setVehicleAngle(vehicleAngle);
        helpers[availabilityMessage->getHostID()].setVehicleSpeed(vehicleSpeed);
        helpers[availabilityMessage->getHostID()].setTaskCpi(tasks[0]->getComputingDensity());
        helpers[availabilityMessage->getHostID()].setVehiclePositionX(vehiclePosX);
        helpers[availabilityMessage->getHostID()].setVehiclePositionY(vehiclePosY);

        int previousAvailability = tasks[0]->getAvailableReceivedCounter();
        previousAvailability++;
        tasks[0]->setAvailableReceivedCounter(previousAvailability);
    }
}

void TaskGenerator::handleResponseMessage(ResponseMessage* responseMessage)
{
    // Search the vehicle in the map
    auto found = helpers.find(responseMessage->getHostIndex());

    // Delete the timer for sending again data message since I've received the message
    auto timer = helpers[responseMessage->getHostIndex()].getTimer(helpers[responseMessage->getHostIndex()].getDataPartitionId());

    if (timer != -1) {
        timerManager.cancel(timer);
    }

    // If the auto is found in the map and the partition id coincide with response message then
    // handle the response otherwise get rid of it
    if (found != helpers.end()) {
        // Emit signal for having received response
        emit(stopResponseMessages, responseMessage->getHostIndex());

        // Cancel and delete the timer message of this vehicle
        // This timer should not be deleted because now is the timer manager that hanldes timers in application
        // cancelAndDelete(helpers[responseMessage->getHostIndex()].getVehicleComputationTimer());

        // Increment the responses I've received
        int responsesReceived = helpers[responseMessage->getHostIndex()].getResponsesReceived();
        helpers[responseMessage->getHostIndex()].setResponsesReceived(++responsesReceived);

        // Remove the data that the vehicle has computed
        double localData = tasks[0]->getTotalData() - responseMessage->getDataComputed();
        tasks[0]->setTotalData(localData);

        EV<<"DATA TASK REMAINED "<<localData<< endl;
        EV<<"HELPERS REMAINED "<<helpers.size()<< endl;
        // If there's no more data then emit signal for task finished
        if (localData <= 0) {
            emit(stopTask, simTime());

            // Color the bus in white
            getParentModule()->getDisplayString().setTagArg("i", 1, "white");

            // Set the load balancing ID to 0
            tasks[0]->setLoadBalancingId(0);

            // Delete the vehicle from vehicles map
            helpers.erase(responseMessage->getHostIndex());
            helpersOrderedList.remove(responseMessage->getHostIndex());

            // Clear the timers
            helpers[responseMessage->getHostIndex()].clearTimers();

            helpers.clear();

            EV<<"END"<<helpers.size()<< endl;
        }

        // Increment the task responses received
        int responseReceived = tasks[0]->getResponseReceivedCounter();
        responseReceived++;
        tasks[0]->setResponseReceivedCounter(responseReceived);

        // Get the load balancing id
        int loadBalanceId = tasks[0]->getLoadBalancingId();

        // Schedule the ack message for each data partition
        if (!(par("useAcks").boolValue())) {
            // Send ACK message to the host
            auto ackMessage = makeShared<AckMessage>();
            ackMessage->setHostIndex(responseMessage->getHostIndex());
            ackMessage->setTaskID(responseMessage->getTaskID());
            ackMessage->setPartitionID(responseMessage->getPartitionID());
            ackMessage->setChunkLength(B(100));
            // L3Address generator = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
            // ackMessage->setSenderAddress(generator);

            auto ackPkt = createPacket("ack_message");
            ackPkt->insertAtBack(ackMessage);
            sendPacket(std::move(ackPkt));
        }

        // If the vehicle is not available anymore erase it from the map
        // and from the list
        if (responseMessage->getStillAvailable() == false) {
            helpers.erase(responseMessage->getHostIndex());
            helpersOrderedList.remove(responseMessage->getHostIndex());
        }

        // If there are more vehicles available and I've received all responses
        // then restart load balancing
        if (helpers.size() > 0 && localData > 0 && totalReponsesExpected == responseReceived) {
            // Increment load balance id
            loadBalanceId++;
            tasks[0]->setLoadBalancingId(loadBalanceId);

            // Set the new availability
            int newAvailability = helpers.size();
            tasks[0]->setAvailableReceivedCounter(newAvailability);

            // Set the responses received and expected to 0
            tasks[0]->setResponseReceivedCounter(0);
            totalReponsesExpected = 0;
            helpers[responseMessage->getHostIndex()].setResponsesReceived(0);

            // Clear the timers
            helpers[responseMessage->getHostIndex()].clearTimers();

            balanceLoad();
        }

        // If there are no more vehicles but still more data to compute then take the bus
        // back in help status and restart vehicles handling to send again help request
        if (helpers.size() == 0 && localData > 0 && totalReponsesExpected == responseReceived) {
            // Color the bus in white when it has no more vehicles
            getParentModule()->getDisplayString().setTagArg("i", 1, "white");

            // Set the new availability
            tasks[0]->setAvailableReceivedCounter(0);

            // Set the responses received and expected to 0
            tasks[0]->setResponseReceivedCounter(0);
            totalReponsesExpected = 0;
            helpers[responseMessage->getHostIndex()].setResponsesReceived(0);

            // Clear the timers
            helpers[responseMessage->getHostIndex()].clearTimers();

            // Change it's status in help
            busState.setState(new Help);

            // Restart the vehicles handling
            vehicleHandler();
        }
    } else if (tasks[0]->getTotalData() <= 0) {
        // If data <= 0 and I receive a response then send ack to the vehicle
        // and delete all the timers
        helpers[responseMessage->getHostIndex()].clearTimers();

        // Set the load balancing ID to 0
        tasks[0]->setLoadBalancingId(0);

        // Set the responses received to 0
        helpers[responseMessage->getHostIndex()].setResponsesReceived(0);

        // Schedule the ack message
        if (!(par("useAcks").boolValue())) {
            // Send ACK message to the host
            auto ackMessage = makeShared<AckMessage>();
            ackMessage->setHostIndex(responseMessage->getHostIndex());
            ackMessage->setTaskID(responseMessage->getTaskID());
            ackMessage->setPartitionID(responseMessage->getPartitionID());
            ackMessage->setChunkLength(B(100));
            // L3Address generator = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
            // ackMessage->setSenderAddress(generator);

            auto ackPkt = createPacket("ack_message");
            ackPkt->insertAtFront(ackMessage);
            sendPacket(std::move(ackPkt));
        }

        helpers.erase(responseMessage->getHostIndex());
        helpersOrderedList.remove(responseMessage->getHostIndex());
    }
}

void TaskGenerator::sendAgainData(DataMessage* data)
{
    // Search the vehicle in the map
    auto found = helpers.find(data->getHostIndex());

    // Check load balancing id
    bool loadBalancingIdCheck = tasks[0]->getLoadBalancingId() == data->getLoadBalancingId();

    // If the vehicle is found check if I've received the data from it
    if (found != helpers.end() && (loadBalancingIdCheck)) {
        // Get the total responses expected
        int responsesExpected = helpers[data->getHostIndex()].getResponsesExpected();
        int responsesReceived = helpers[data->getHostIndex()].getResponsesReceived();

        // Check the responses
        bool checkResponses = responsesExpected != responsesReceived;

        if (checkResponses) {
            auto newData = makeShared<DataMessage>();

            // Fill fields of data message with previous data message

            /**************************************************************************
             * This has to be done because in veins if you send a message duplicate   *
             * it will be discarded from MAC L2 because it's a message that the       *
             * single node "read" as already received                                 *
             *************************************************************************/

            newData->setChunkLength(B(data->getChunkLength()));
            newData->setLoadToProcess(data->getLoadToProcess());
            // L3Address generator = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
            // newData->setSenderAddress(generator);
            newData->setHostIndex(data->getHostIndex());
            newData->setTaskId(data->getTaskId());
            newData->setTaskSize(data->getTaskSize());
            newData->setPartitionId(data->getPartitionId());
            newData->setLoadBalancingId(data->getLoadBalancingId());
            newData->setCpi(data->getCpi());
            newData->setComputationTime(data->getComputationTime());
            newData->setResponsesExpected(data->getResponsesExpected());

            auto newDataPkt = createPacket("send_again_data");
            newDataPkt->insertAtBack(newData);

            // Send the duplicate data message
            sendPacket(std::move(newDataPkt));

            double transferTime = 10.0;

            double time = (transferTime + data->getComputationTime() + par("dataComputationThreshold").doubleValue());

            // The & inside the square brackets tells to capture all local variable
            // by value
            auto callback = [=]() {
                sendAgainData(data->dup());
            };
            // Rewrite the occurence of timer in the timer map
            veins::TimerManager::TimerHandle timer = timerManager.create(veins::TimerSpecification(callback).oneshotIn(time));
            helpers[data->getHostIndex()].addTimer(data->getPartitionId(), timer);
        }
    }
}
