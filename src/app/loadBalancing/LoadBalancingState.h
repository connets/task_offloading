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

#ifndef APP_LOADBALANCING_LOADBALANCINGSTATE_H_
#define APP_LOADBALANCING_LOADBALANCINGSTATE_H_

namespace tirocinio {
    class LoadBalancingState
    {
        public:
            virtual bool getCurrentState() = 0;
            virtual ~LoadBalancingState();
    };

    class Active : public LoadBalancingState
    {
        public:
            bool getCurrentState();
    };

    class Disabled : public LoadBalancingState
    {
        public:
            bool getCurrentState();
    };

    class LoadBalancingContext {
        public:
            LoadBalancingContext();
            LoadBalancingContext(LoadBalancingState* newState);
            void setState(LoadBalancingState* newState);
            bool getCurrentState();

        protected:
            LoadBalancingState* currentState;
    };
}

#endif /* APP_LOADBALANCING_LOADBALANCINGSTATE_H_ */
