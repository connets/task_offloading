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

#include "ComputationTime.h"

using namespace task_offloading;

Define_Module(task_offloading::ComputationTime);

ComputationTime::ComputationTime() {
    //

}

ComputationTime::~ComputationTime() {
    //
}

bool cmpComputationTime(std::pair<int, HelperVehicleInfo> &a, std::pair<int, HelperVehicleInfo> &b) {
    return a.second.getTotalComputationTime(3) < b.second.getTotalComputationTime(3);
}

std::list<int> ComputationTime::sort(const std::map<int, HelperVehicleInfo> &map) {
    std::list<int> l;

    std::vector<std::pair<int, HelperVehicleInfo>> pairVector;

    // Copy the IDs of the map into the vector
    for (auto &it : map) {
        pairVector.push_back(it);
    }

    // Sorting the array with the cmp function defined above
    std::sort(pairVector.begin(), pairVector.end(), cmpComputationTime);

    // Fill the list with ordered IDs
    for (auto &it : pairVector) {
        l.push_back(it.first);
    }

    return l;
}
