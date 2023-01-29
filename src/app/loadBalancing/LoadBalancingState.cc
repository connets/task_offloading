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

#include "LoadBalancingState.h"

using namespace task_offloading;

LoadBalancingState::~LoadBalancingState()
{
    //
}

LoadBalancingContext::LoadBalancingContext()
{
    currentState = new Disabled;
}

LoadBalancingContext::LoadBalancingContext(LoadBalancingState* newState)
{
    currentState = newState;
}

void LoadBalancingContext::setState(LoadBalancingState* newState)
{
    if (currentState) {
        delete currentState;
        currentState = newState;
    }
}

bool LoadBalancingContext::getCurrentState()
{
    return currentState->getCurrentState();
}

bool Active::getCurrentState()
{
    return false;
}

bool Disabled::getCurrentState()
{
    return true;
}
