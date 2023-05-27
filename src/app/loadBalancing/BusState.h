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

#ifndef APP_LOADBALANCING_BUSSTATE_H_
#define APP_LOADBALANCING_BUSSTATE_H_

namespace task_offloading {
    class BusState
    {
        public:
            virtual int getCurrentState() = 0;
            virtual ~BusState();
    };

    class Help : public BusState
    {
        public:
            int getCurrentState();
    };

    class LoadBalancing : public BusState
    {
        public:
            int getCurrentState();
    };

    class DataTransfer : public BusState
    {
        public:
            int getCurrentState();
    };

    class FinishedComputation : public BusState
    {
        public:
            int getCurrentState();
    };

    class BusContext {
        public:
            BusContext();
            BusContext(BusState* newState);
            virtual ~BusContext();
            void setState(BusState* newState);
            int getCurrentState();

        protected:
            BusState* currentState;
    };
}

#endif /* APP_LOADBALANCING_BUSSTATE_H_ */
