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

package task_offloading.simulations.task_offloading;

import org.car2x.veins.nodes.RSU;
import org.car2x.veins.nodes.Scenario;
import task_offloading.app.VeinsApp;
import task_offloading.app.loadBalancing.sortingAlgorithm.ISorting;

network task_offloading extends Scenario
{
    @display("bgb=3500,3500");
    submodules:
        rsu[1]: RSU {
            @display("p=170,140;i=veins/sign/yellowdiamond;is=vs");
        }

        veinsApp: VeinsApp {
            @display("p=98,140");
        }
        
        loadBalancingAlgorithm: <default("FIFO")> like ISorting {
            @display("i=block/cogwheel;p=248,140");
        }
}
