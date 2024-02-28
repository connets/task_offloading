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

#include "veins/veins.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "app/messages/ComputationTimerMessage_m.h"
#include "inet/networklayer/common/L3Address.h"

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
        double getTotalComputationTime(int CPI);
        double getTotalComputationTime(int CPI, double load);
    };
}

#endif /* APP_VEHICLESHANDLING_HELPERVEHICLEINFO_H_ */
