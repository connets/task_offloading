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

#pragma once

#include "veins_inet/veins_inet.h"

using namespace omnetpp;

namespace task_offloading {
    class Task : public cSimpleModule {
        public:
            void initialize() override;
            void finish() override;
            int getId();
            double getTotalData();
            int getHelpReceivedCounter();
            long getDataPartitionId();
            int getLoadBalancingId();
            int getAvailableReceivedCounter();
            int getResponseReceivedCounter();
            double getMinimumLoadRequested();
            int getComputingDensity();
            void setId(int newId);
            void setTotalData(double newData);
            void setHelpReceivedCounter(int newCounter);
            void setDataPartitionId(long newDataPartitionId);
            void setLoadBalancingId(int newLoadBalancingId);
            void setAvailableReceivedCounter(int newCounter);
            void setResponseReceivedCounter(int newCounter);
            void setMinimumLoadRequested(double newMinimumLoad);
            void setComputingDensity(int newCpi);
            void insertDataPartition(int partitionId);
            int getDataPartition(int partitionId);
            void removeDataPartition(int partitionId);

            // Signals
            simsignal_t totalTaskTime;
            simsignal_t loadBalancingTime;
            simsignal_t loadBalancingRound;
            simsignal_t totalMessagesGenerator;
            simsignal_t totalRetransimissionsGenerator;
            simsignal_t totalVehiclesAvailable;

        private:
            int id;
            double totalData;
            double minimumLoadRequested;
            int computingDensity;
            int helpReceivedCounter;
            long dataPartitionId;
            int loadBalancingId;
            int availableReceivedCounter;
            int responseReceivedCounter;
            std::map<int, int> dataPartitions;
    };
}
