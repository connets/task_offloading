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

#ifndef APP_LOADBALANCING_SORTINGALGORITHM_COMPUTATIONTIME_H_
#define APP_LOADBALANCING_SORTINGALGORITHM_COMPUTATIONTIME_H_

#include "veins/veins.h"
#include "app/vehiclesHandling/HelperVehicleInfo.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

namespace task_offloading {
    class ComputationTime : public cSimpleModule {
    public:
        ComputationTime();
        virtual ~ComputationTime();
        std::list<int> sort(const std::map<int, HelperVehicleInfo> &map);
    };
}

#endif /* APP_LOADBALANCING_SORTINGALGORITHM_COMPUTATIONTIME_H_ */
