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

#ifndef APP_VEHICLESHANDLING_BUSVEHICLEINFO_H_
#define APP_VEHICLESHANDLING_BUSVEHICLEINFO_H_

#include "veins/veins.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

namespace task_offloading {
    class BusVehicleInfo {
    private:
        std::string index;
        simtime_t createdAt;
        double vehicleSpeed;
        double vehicleAngle;
        double vehiclePositionX;
        double vehiclePositionY;


    public:
        BusVehicleInfo();
        BusVehicleInfo(std::string index);
        virtual ~BusVehicleInfo();

        std::string getIndex();
        simtime_t getCreatedAt();
        double getVehicleAngle();
        double getVehiclePositionX();
        double getVehiclePositionY();
        double getVehicleSpeed();

        void setIndex(std::string index);
        void setCreatedAt(simtime_t newTime);
        void setVehicleAngle(double newAngle);
        void setVehiclePositionX(double newPositionX);
        void setVehiclePositionY(double newPositionY);
        void setVehicleSpeed(double newSpeed);
    };
}


#endif /* APP_VEHICLESHANDLING_BUSVEHICLEINFO_H_ */
