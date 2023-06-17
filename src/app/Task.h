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

#ifndef APP_TASK_H_
#define APP_TASK_H_

#include "veins/veins.h"
#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"

namespace task_offloading {
    class Task {
        public:
            Task();
            Task(int id, double totalData, double loadRequested, int computingDensity);
            int getId();
            double getTotalData();
            int getHelpReceivedCounter();
            int getDataPartitionId();
            int getLoadBalancingId();
            int getAvailableReceivedCounter();
            int getResponseReceivedCounter();
            double getMinimumLoadRequested();
            int getComputingDensity();
            void setId(int newId);
            void setTotalData(double newData);
            void setHelpReceivedCounter(int newCounter);
            void setDataPartitionId(int newDataPartitionId);
            void setLoadBalancingId(int newLoadBalancingId);
            void setAvailableReceivedCounter(int newCounter);
            void setResponseReceivedCounter(int newCounter);
            void setMinimumLoadRequested(double newMinimumLoad);
            void setComputingDensity(int newCpi);
            virtual ~Task();

        private:
            int id;
            double totalData;
            double minimumLoadRequested;
            int computingDensity;
            int helpReceivedCounter;
            int dataPartitionId;
            int loadBalancingId;
            int availableReceivedCounter;
            int responseReceivedCounter;
    };
}
#endif /* APP_TASK_H_ */
