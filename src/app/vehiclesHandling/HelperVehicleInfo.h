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

namespace task_offloading {
    class HelperVehicleInfo {
    private:
        std::string index;
        double hostCurrentLoad;
        double hostCPUFreq;
        simtime_t createdAt;
        veins::LAddress::L2Type address;
        double vehicleSpeed;
        double vehicleAngle;
        double vehiclePositionX;
        double vehiclePositionY;
        int dataPartitionId;
        int taskCpi;

    public:
        HelperVehicleInfo();
        HelperVehicleInfo(std::string index, double load, double freq, veins::LAddress::L2Type address);
        virtual ~HelperVehicleInfo();
        std::string getIndex();
        double getCurrentLoad();
        double getCPUFreq();
        simtime_t getCreatedAt();
        veins::LAddress::L2Type getAddress();
        double getVehicleAngle();
        int getDataPartitionId();
        int getTaskCpi();
        double getVehicleSpeed();
        double getVehiclePositionX();
        double getVehiclePositionY();
        void setIndex(std::string index);
        void setCurrentLoad(double newLoad);
        void setCPUFreq(double newFreq);
        void setCreatedAt(simtime_t newTime);
        void setAddress(veins::LAddress::L2Type newAddress);
        void setVehicleAngle(double newAngle);
        void setDataPartitionId(int newPartitionId);
        void setTaskCpi(int newCpi);
        void setVehicleSpeed(double newSpeed);
        void setVehiclePositionX(double newVehiclePositionX);
        void setVehiclePositionY(double newVehiclePositionY);
        double getTotalComputationTime(int CPI);
    };
}

#endif /* APP_VEHICLESHANDLING_HELPERVEHICLEINFO_H_ */
