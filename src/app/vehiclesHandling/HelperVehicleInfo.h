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

#ifndef APP_VEHICLESHANDLING_HELPERVEHICLEINFO_H_
#define APP_VEHICLESHANDLING_HELPERVEHICLEINFO_H_

#include "veins_inet/veins_inet.h"
#include "veins/veins.h"
#include "veins_inet/VeinsInetApplicationBase.h"
#include "app/messages/ComputationTimerMessage_m.h"
#include "inet/networklayer/common/L3Address.h"
#include <iomanip>
#include <random>
#include <map>
#include <iostream>

namespace task_offloading {
    class HelperVehicleInfo {
    private:
        std::string index;
        double hostCurrentLoad;
        double hostCPUFreq;
        simtime_t createdAt;
        inet::L3Address address;
        double vehicleSpeed;
        double vehicleAngle;
        double vehiclePositionX;
        double vehiclePositionY;
        int dataPartitionId;
        int taskCpi;
        double vehicleComputationTimer;
        int responsesExpected;
        int responsesReceived;
        std::map<int, veins::TimerManager::TimerHandle> timers;

    public:
        HelperVehicleInfo();
        HelperVehicleInfo(std::string index, double load, double freq, inet::L3Address address);
        virtual ~HelperVehicleInfo();
        std::string getIndex();
        double getCurrentLoad();
        double getCPUFreq();
        simtime_t getCreatedAt();
        inet::L3Address getAddress();
        double getVehicleAngle();
        int getDataPartitionId();
        int getTaskCpi();
        double getVehicleSpeed();
        double getVehiclePositionX();
        double getVehiclePositionY();
        double getVehicleComputationTimer();
        int getResponsesExpected();
        int getResponsesReceived();
        void setIndex(std::string index);
        void setCurrentLoad(double newLoad);
        void setCPUFreq(double newFreq);
        void setCreatedAt(simtime_t newTime);
        void setAddress(inet::L3Address newAddress);
        void setVehicleAngle(double newAngle);
        void setDataPartitionId(int newPartitionId);
        void setTaskCpi(int newCpi);
        void setVehicleSpeed(double newSpeed);
        void setVehiclePositionX(double newVehiclePositionX);
        void setVehiclePositionY(double newVehiclePositionY);
        void setVehicleComputationTimer(double newComputationTimer);
        void setResponsesExpected(int newTotal);
        void setResponsesReceived(int newTotal);
        double getTotalComputationTime(int CPI);
        double getTotalComputationTime(int CPI, double load);
        void addTimer(int partitionID, veins::TimerManager::TimerHandle timer);
        veins::TimerManager::TimerHandle getTimer(int partitionID);
        void clearTimers();
    };
}

#endif /* APP_VEHICLESHANDLING_HELPERVEHICLEINFO_H_ */
