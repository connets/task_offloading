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
#include "app/messages/LoadBalanceTimerMessage_m.h"

using namespace task_offloading;

void TaskGenerator::vehicleHandler()
{
    // Get the timer for the first help message
    bool timerForFirstHelpMessage = simTime() > par("randomTimeFirstHelpMessage").doubleValue();
    // Check the bus state
    int currentBusState = busState.getCurrentState();
    // Check if there's more data
    bool moreDataToLoad = tasks[0].getData() > 0;
    // Check if it's the second (or more) help message
    bool isFirstHelpMessage = tasks[0].getHelpReceivedCounter() ==  0;

    // If it's the first message then initialize the task
    if (isFirstHelpMessage) {
        tasks[0] = Task(0, par("computationLoad").doubleValue(), par("minimumVehicleLoadRequested").doubleValue(), 3);
    }

    // Check if we reach the time of the first help message
    if (timerForFirstHelpMessage && (currentBusState == 0) && moreDataToLoad) {
        // Color the bus icon in red
        findHost()->getDisplayString().setTagArg("i", 1, "red");

        // Prepare the help message
        HelpMessage* helpMessage = new HelpMessage();

        // Populate the message
        populateWSM(helpMessage);
        helpMessage->setId(tasks[0].getHelpReceivedCounter());
        helpMessage->setVehicleIndex(busIndex);
        helpMessage->setMinimumLoadRequested(tasks[0].getMinimumLoadRequested());

        // Emit signal for start help message
        emit(startHelp, simTime());

        // Send the message in broadcast
        sendDown(helpMessage);

        if (tasks[0].getHelpReceivedCounter() == 0) {
            emit(startTask, simTime());
        }

        // Start bus waiting timer for accepting availability messages
        LoadBalanceTimerMessage* timerForLoadBalancing = new LoadBalanceTimerMessage();

        // Save the actual simtime for future help messages
        simtime_t simTimeActual = simTime();

        // Populate the message
        populateWSM(timerForLoadBalancing);
        timerForLoadBalancing->setSimulationTime(simTimeActual);

        // Change the load balancing state
        busState.setState(new LoadBalancing);

        // Increment the counter for help messages
        int helpCounter = tasks[0].getHelpReceivedCounter();
        helpCounter++;
        tasks[0].setHelpReceivedCounter(helpCounter);

        // Schedule the message -> simTime + availability msgs threshold
        scheduleAt(simTimeActual + par("busWaitingTimeForAvailability").doubleValue(), timerForLoadBalancing);
    } else if (tasks[0].getData() <= 0) {
        // Color the bus in white when computation ends
        findHost()->getDisplayString().setTagArg("i", 1, "white");
    }
}
