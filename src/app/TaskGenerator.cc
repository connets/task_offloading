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
        busIndex = getParentModule()->getIndex();

        // Initialize the BUS state
        busState = BusContext(new Help);

        // Initialize the load balancing algorithm
        loadBalancingAlgorithm = check_and_cast<BaseSorting*>(findModuleByPath("task_offloading.loadBalancingAlgorithm"));

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
    TimerFirstHelpMessage* timer = new TimerFirstHelpMessage();
    simtime_t time = simTime() + par("randomTimeFirstHelpMessage").doubleValue();
    scheduleAfter(time, timer);
}

void TaskGenerator::sendUnicastPacket(std::unique_ptr<inet::Packet> pk, inet::L3Address destAddress, int portNumber)
{
    socket.sendTo(pk.release(), destAddress, portNumber);
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

void TaskGenerator::handleMessageWhenUp(inet::cMessage* msg)
{
    /************************************************************************
      Your application has received a data message itself (self-message)
    ************************************************************************/
    // Check if the message is a self message
    if (msg->isSelfMessage()) {
        // Timer for the first help message
        if (TimerFirstHelpMessage* loadBalanceMsg = dynamic_cast<TimerFirstHelpMessage*>(msg)) {
            vehicleHandler();
        }

        // Timer for future help messages
        if (LoadBalanceTimerMessage* loadBalanceMsg = dynamic_cast<LoadBalanceTimerMessage*>(msg)) {
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
        }

        // Timer for data computation
        if (ComputationTimerMessage* computationTimerMessage = dynamic_cast<ComputationTimerMessage*>(msg)) {
            DataMessage* data = (DataMessage*)computationTimerMessage->getData();
            sendAgainData(data);
        }

        // Timer for data message
        if (DataMessage* dataMsg = dynamic_cast<DataMessage*>(msg)) {
            // Send signal for data message statistic with the host ID
            emit(startDataMessages, dataMsg->getHostIndex());

            // Create data payload
            auto data = dataMsg->dupShared();

            // Create the data packet
            auto dataPacket = createPacket("duplicate_data");
            dataPacket->insertAtBack(data);
            int servicePort = par("servicePort").intValue();
            L3Address destAddress = dataMsg->getSenderAddress();
            sendUnicastPacket(std::move(dataPacket), destAddress, servicePort);
        }

        // Timer for ACK message
        if (AckMessage* ackMsg = dynamic_cast<AckMessage*>(msg)) {
            // Create data payload
            auto ack = ackMsg->dupShared();

            // Create the data packet
            auto ackPacket = createPacket("duplicate_ack");
            ackPacket->insertAtBack(ack);
            int servicePort = par("servicePort").intValue();
            L3Address destAddress = ackMsg->getSenderAddress();
            sendUnicastPacket(std::move(ackPacket), destAddress, servicePort);
        }
    } else {
        /************************************************************************
          Your application has received a data message from another car or RSU
        ************************************************************************/

        // SECTION - When the bus receives the ok messages
        if (AvailabilityMessage* availabilityMessage = dynamic_cast<AvailabilityMessage*>(msg)) {
            handleAvailabilityMessage(availabilityMessage);
        }

        // SECTION - When the bus receive the response message
        if (ResponseMessage* responseMessage = dynamic_cast<ResponseMessage*>(msg)) {
            handleResponseMessage(responseMessage);
        }

        if(BeaconMessage* bmsg = dynamic_cast<BeaconMessage*>(msg)) {
            emit(stopBeaconMessages, simTime());
        }
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
    double localData = tasks[0].getTotalData();

    // Emit the start of load balancing
    emit(startBalance, simTime());

    // For each vehicle prepare the data message and send
    for (auto const &i: helpersOrderedList) {
        // Check if there's data to load
        if (localData > 0) {
            // Prepare the data message
            // DataMessage* dataMessage = new DataMessage();
            auto dataMessage = makeShared<DataMessage>();

            // Populate the data message
            // Check if data is > 0 then update the local data variable
            if ((localData - helpers[i].getCurrentLoad()) > 0) {
                // Set the byte length
                dataMessage->setChunkLength(B(helpers[i].getCurrentLoad()));

                // Set the message load to process
                dataMessage->setLoadToProcess(helpers[i].getCurrentLoad());
                localData = localData - helpers[i].getCurrentLoad();
            } else {
                // Set the byte length
                dataMessage->setChunkLength(B(localData));

                // Set the message load to process
                dataMessage->setLoadToProcess(localData);
                localData = 0;
            }

            // Populate the other fields
            L3Address generator = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
            dataMessage->setSenderAddress(generator);
            dataMessage->setHostIndex(i);
            dataMessage->setTaskId(tasks[0].getId());
            dataMessage->setTaskSize(tasks[0].getTotalData());
            dataMessage->setPartitionId(tasks[0].getDataPartitionId());
            dataMessage->setLoadBalancingId(tasks[0].getLoadBalancingId());
            dataMessage->setCpi(tasks[0].getComputingDensity());

            // Calculate time for timer
            double CPI = tasks[0].getComputingDensity();
            double timeToCompute = helpers[i].getTotalComputationTime(CPI);

            dataMessage->setComputationTime(timeToCompute);

            // Get the current data partition id
            int currentPartitionId = tasks[0].getDataPartitionId();

            // Save into the helper the data partition ID
            helpers[i].setDataPartitionId(currentPartitionId);

            // Create timer computation message for each host if auto ACKs are disabled
            if (par("useAcks").boolValue() == false) {
                ComputationTimerMessage* computationTimerMessage = new ComputationTimerMessage();
                computationTimerMessage->setData(dataMessage->dup());

                // Calculate time to file transmission
                //Calculate bitrate conversion from megabit to megabyte
                double bitRate = getModuleByPath(".^.nic.mac1609_4")->par("bitrate").intValue() / 8.0;
                double transferTime = localData/bitRate;

                // Save the computation timer into helpers map
                helpers[i].setVehicleComputationTimer(computationTimerMessage);

                scheduleAfter(timeToCompute + transferTime + par("dataComputationThreshold").doubleValue(), computationTimerMessage);
            }

            // Schedule the data packet
            auto dataPacket = createPacket("data_message");
            dataPacket->insertAtBack(dataMessage);
            int servicePort = par("servicePort").intValue();
            L3Address destAddress = helpers[i].getAddress();
            sendUnicastPacket(std::move(dataPacket), destAddress, servicePort);

            // Increment data partition ID
            currentPartitionId++;
            tasks[0].setDataPartitionId(currentPartitionId);
        }
    }

    // Change the bus state to data transfer
    busState.setState(new DataTransfer);

    // Emit the stop of load balancing
    emit(stopBalance, simTime());
}

void TaskGenerator::vehicleHandler()
{
    EV << "test_start_application" << std::endl;
    // Get the timer for the first help message
    // bool timerForFirstHelpMessage = simTime() > par("randomTimeFirstHelpMessage").doubleValue();
    // Check the bus state
    int currentBusState = busState.getCurrentState();
    // Check if there's more data
    bool moreDataToLoad = tasks[0].getTotalData() > 0;
    // Check if it's the second (or more) help message
    bool isFirstHelpMessage = tasks[0].getHelpReceivedCounter() ==  0;

    // Check if we reach the time of the first help message
    if ((currentBusState == 0) && moreDataToLoad && helpers.size() == 0) {
        // If it's the first message then initialize the task
        if (isFirstHelpMessage) {
            tasks[0] = Task(0, par("computationLoad").doubleValue(), par("minimumVehicleLoadRequested").doubleValue(), 3);
        }

        // Color the bus icon in red
        getParentModule()->getDisplayString().setTagArg("i", 1, "red");

        // Prepare the help message
        auto helpMessage = makeShared<HelpMessage>();

        // Populate the message
        helpMessage->setId(tasks[0].getId());
        helpMessage->setVehicleIndex(busIndex);
        helpMessage->setCpi(tasks[0].getComputingDensity());
        helpMessage->setTaskSize(tasks[0].getTotalData());
        helpMessage->setMinimumLoadRequested(tasks[0].getMinimumLoadRequested());
        helpMessage->setChunkLength(B(200));

        // Emit signal for start help message
        emit(startHelp, simTime());

        // Send the packet in broadcast
        auto packet = createPacket("help_message");
        packet->insertAtBack(helpMessage);
        int servicePort = par("servicePort").intValue();
        sendUnicastPacket(std::move(packet), destAddress, portNumber);

        if (tasks[0].getHelpReceivedCounter() == 0) {
            emit(startTask, simTime());
        }

        // Start bus waiting timer for accepting availability messages
        LoadBalanceTimerMessage* timerForLoadBalancing = new LoadBalanceTimerMessage();

        // Save the actual simtime for future help messages
        simtime_t simTimeActual = simTime();

        // Populate the message
        timerForLoadBalancing->setSimulationTime(simTimeActual);

        // Change the load balancing state
        busState.setState(new LoadBalancing);

        // Increment the counter for help messages
        int helpCounter = tasks[0].getHelpReceivedCounter();
        helpCounter++;
        tasks[0].setHelpReceivedCounter(helpCounter);

        // Schedule the message -> simTime + availability msgs threshold
        scheduleAt(simTimeActual + par("busWaitingTimeForAvailability").doubleValue(), timerForLoadBalancing);
    } else if (tasks[0].getTotalData() <= 0) {
        // Color the bus in white when computation ends
        getParentModule()->getDisplayString().setTagArg("i", 1, "white");
    }
}

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
        aRt = (transferTime + timeToCompute + transferTimeRes)*par("retryFactorTime").doubleValue();

    }
    //car position at the current time
    double cx = helpers[availabilityMessage->getHostID()].getVehiclePositionX();
    double cy = helpers[availabilityMessage->getHostID()].getVehiclePositionY();
    //bus position at the current time
    veins::TraCIMobility* mobilityMod = check_and_cast<veins::TraCIMobility*>(getModuleByPath("^.veinsmobility"));
    double bx = mobilityMod->getPositionAt(simTime()).x;
    double by = mobilityMod->getPositionAt(simTime()).y;

    //future positions of car (next aRt seconds)
    double nextCx = (helpers[availabilityMessage->getHostID()].getVehicleSpeed()*aRt*std::cos(helpers[availabilityMessage->getHostID()].getVehicleAngle())) + cx;
    double nextCy = (helpers[availabilityMessage->getHostID()].getVehicleSpeed()*aRt*std::sin(helpers[availabilityMessage->getHostID()].getVehicleAngle())) + cy;
    //future positions of car (next aRt seconds)
    double nextBx = (traciVehicle->getSpeed()*aRt*std::cos(traciVehicle->getAngle())) + bx;
    double nextBy = (traciVehicle->getSpeed()*aRt*std::sin(traciVehicle->getAngle())) + by;

    //radius for both the car and bus reach in their next aRt seconds
//    double radiusCar = sqrt(pow((nextCx - cx),2.0) + pow((nextCy - cy),2.0));
    double radiusBus = sqrt(pow((nextBx - bx),2.0) + pow((nextBy - by),2.0));

    //check if both the bus and the car will be in each other reach in the next aRt seconds
    //((x-center_x)^2 + (y-center_y)^2 < radius^2)
    if(!(pow(nextCx-nextBx,2.0) + pow(nextCy-nextBy,2.0) < pow(radiusBus,2.0))) {
        //remove vehicle availability due to projected long distance
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
        helpers[availabilityMessage->getHostID()].setTaskCpi(tasks[0].getComputingDensity());
        helpers[availabilityMessage->getHostID()].setVehiclePositionX(vehiclePosX);
        helpers[availabilityMessage->getHostID()].setVehiclePositionY(vehiclePosY);

        int previousAvailability = tasks[0].getAvailableReceivedCounter();
        previousAvailability++;
        tasks[0].setAvailableReceivedCounter(previousAvailability);
    }
}

void TaskGenerator::handleResponseMessage(ResponseMessage* responseMessage)
{
    // Search the vehicle in the map
    auto found = helpers.find(responseMessage->getHostIndex());

    // If the auto is found in the map and the partition id coincide with response message then
    // handle the response otherwise get rid of it
    if (found != helpers.end() && helpers[responseMessage->getHostIndex()].getDataPartitionId() == responseMessage->getPartitionID()) {
        // Emit signal for having received response
        emit(stopResponseMessages, responseMessage->getHostIndex());

        // Cancel and delete the timer message of this vehicle
        cancelAndDelete(helpers[responseMessage->getHostIndex()].getVehicleComputationTimer());

        // Update the data partiton id into the helpers map
        helpers[responseMessage->getHostIndex()].setDataPartitionId(-1);

        // Remove the data that the vehicle has computed
        double localData = tasks[0].getTotalData() - responseMessage->getDataComputed();
        tasks[0].setTotalData(localData);

        EV<<"DATA TASK REMAINED "<<localData<< endl;
        EV<<"HELPERS REMAINED "<<helpers.size()<< endl;
        // If there's no more data then emit signal for task finished
        if (localData <= 0) {
            emit(stopTask, simTime());
            EV<<"END"<<helpers.size()<< endl;
        }

        // Increment the task responses received
        int responseReceived = tasks[0].getResponseReceivedCounter();
        responseReceived++;
        tasks[0].setResponseReceivedCounter(responseReceived);

        // Get the availability received
        int vehiclesAvailable = tasks[0].getAvailableReceivedCounter();

        // Get the load balancing id
        int loadBalanceId = tasks[0].getLoadBalancingId();

        // If the vehicle is not available anymore erase it from the map
        // and from the list
        if (responseMessage->getStillAvailable() == false) {
            // Schedule the ack message
            if (!(par("useAcks").boolValue())) {
                // Send ACK message to the host
                auto ackMessage = makeShared<AckMessage>();
                ackMessage->setHostIndex(responseMessage->getHostIndex());
                ackMessage->setTaskID(responseMessage->getTaskID());
                ackMessage->setPartitionID(responseMessage->getPartitionID());
                L3Address generator = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
                ackMessage->setSenderAddress(generator);

                auto ackPkt = createPacket("ACK");
                ackPkt->insertAtBack(ackMessage);
                int servicePort = par("servicePort").intValue();
                L3Address destAddress = responseMessage->getSenderAddress();
                sendUnicastPacket(std::move(ackPkt), destAddress, servicePort);
            }

            helpers.erase(responseMessage->getHostIndex());
            helpersOrderedList.remove(responseMessage->getHostIndex());
        }

        // If there are more vehicles available and I've received all responses
        // then restart load balancing
        if (helpers.size() > 0 && localData > 0 && vehiclesAvailable == responseReceived) {
            // Increment load balance id
            loadBalanceId++;
            tasks[0].setLoadBalancingId(loadBalanceId);

            // Set the new availability
            int newAvailability = helpers.size();
            tasks[0].setAvailableReceivedCounter(newAvailability);

            // Set the responses received to 0
            tasks[0].setResponseReceivedCounter(0);

            balanceLoad();
        }

        // If there are no more vehicles but still more data to compute then take the bus
        // back in help status
        if (helpers.size() == 0 && localData > 0 && vehiclesAvailable == responseReceived) {
            // Color the bus in white when it has no more vehicles
            getParentModule()->getDisplayString().setTagArg("i", 1, "white");

            // Set the new availability
            tasks[0].setAvailableReceivedCounter(0);

            // Set the responses received to 0
            tasks[0].setResponseReceivedCounter(0);

            // Change it's status in help
            busState.setState(new Help);
        }
    } else if (tasks[0].getTotalData() <= 0) {
        // If data <= 0 and I receive a response then send ack to the vehicle

        // Schedule the ack message
        if (!(par("useAcks").boolValue())) {
            // Send ACK message to the host
            auto ackMessage = makeShared<AckMessage>();
            ackMessage->setHostIndex(responseMessage->getHostIndex());
            ackMessage->setTaskID(responseMessage->getTaskID());
            ackMessage->setPartitionID(responseMessage->getPartitionID());
            L3Address generator = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
            ackMessage->setSenderAddress(generator);

            auto ackPkt = createPacket("ACK");
            ackPkt->insertAtFront(ackMessage);
            int servicePort = par("servicePort").intValue();
            L3Address destAddress = responseMessage->getSenderAddress();
            sendUnicastPacket(std::move(ackPkt), destAddress, servicePort);
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
    bool loadBalancingIdCheck = tasks[0].getLoadBalancingId() == data->getLoadBalancingId();

    // If the vehicle is found check if I've received the data from it
    if (found != helpers.end() && (loadBalancingIdCheck)) {
        // Check the data partition id
        bool checkDataPartitionId = helpers[data->getHostIndex()].getDataPartitionId() != -1;

        if (checkDataPartitionId) {
            auto newData = makeShared<DataMessage>();

            // Fill fields of data message with previous data message

            /**************************************************************************
             * This has to be done because in veins if you send a message duplicate   *
             * it will be discarded from MAC L2 because it's a message that the       *
             * single node "read" as already received                                 *
             *************************************************************************/

            newData->setChunkLength(B(data->getChunkLength()));
            newData->setLoadToProcess(data->getLoadToProcess());
            L3Address generator = getModuleFromPar<Ipv4InterfaceData>(par("interfaceTableModule"), this)->getIPAddress();
            newData->setSenderAddress(generator);
            newData->setHostIndex(data->getHostIndex());
            newData->setTaskId(data->getTaskId());
            newData->setTaskSize(data->getTaskSize());
            newData->setPartitionId(data->getPartitionId());
            newData->setLoadBalancingId(data->getLoadBalancingId());
            newData->setCpi(data->getCpi());
            newData->setComputationTime(data->getComputationTime());

            auto newDataPkt = createPacket("send_again_data");
            newDataPkt->insertAtBack(newData);
            int servicePort = par("servicePort").intValue();
            L3Address destAddress = helpers[data->getHostIndex()].getAddress();

            // Send the duplicate data message
            sendUnicastPacket(std::move(newDataPkt), destAddress, servicePort);

            // Restart again the timer
            ComputationTimerMessage* computationTimerMessage = new ComputationTimerMessage();
            computationTimerMessage->setData(data->dup());

            double transferTime = 10.0;

            scheduleAt(simTime() + transferTime + data->getComputationTime() + par("dataComputationThreshold").doubleValue(), computationTimerMessage);
        }
    }
}
