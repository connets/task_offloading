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

#include "FIFO.h"

using namespace task_offloading;

Define_Module(task_offloading::FIFO);

FIFO::FIFO() {
    //
}

FIFO::~FIFO() {
    //
}

bool cmpFIFO(std::pair<const char *, HelperVehicleInfo> &a, std::pair<const char *, HelperVehicleInfo> &b) {
    return a.second.getCreatedAt() < b.second.getCreatedAt();
}

std::list<const char *> FIFO::sort(const std::map<const char *, HelperVehicleInfo> &map) {
    std::list<const char *> l;

    std::vector<std::pair<const char *, HelperVehicleInfo>> pairVector;

    // Copy the IDs of the map into the vector
    for (auto &it : map) {
        pairVector.push_back(it);
    }

    // Sorting the array with the cmp function defined above
    std::sort(pairVector.begin(), pairVector.end(), cmpFIFO);

    // Fill the list with ordered IDs
    for (auto &it : pairVector) {
        l.push_back(it.first);
    }

    return l;
}
