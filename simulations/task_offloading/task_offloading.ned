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

import task_offloading.app.Worker;
import task_offloading.app.TaskGenerator;
import task_offloading.app.Beaconer;
import task_offloading.app.loadBalancing.sortingAlgorithm.ISorting;
import org.car2x.veins.base.connectionManager.ConnectionManager;
import org.car2x.veins.base.modules.BaseWorldUtility;
import org.car2x.veins.subprojects.veins_inet.VeinsInetManager;
import org.car2x.veins.modules.obstacle.ObstacleControl;
import org.car2x.veins.modules.world.annotations.AnnotationManager;
import org.car2x.veins.visualizer.roads.RoadsCanvasVisualizer;
import inet.physicallayer*.wireless.ieee80211.packetlevel.Ieee80211DimensionalRadioMedium;

network task_offloading
{
    parameters:
        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)
        @display("bgb=3500,3500");
    submodules:
        radioMedium: Ieee80211DimensionalRadioMedium {
            @display("p=64,224");
        }
        
        loadBalancingAlgorithm: <default("FIFO")> like ISorting {
            @display("i=block/cogwheel;p=248,140");
        }

        obstacles: ObstacleControl {
            @display("p=616.704,378.43198");
        }
        
        annotations: AnnotationManager {
            @display("p=644.73596,378.43198");
        }
        
        connectionManager: ConnectionManager {
            parameters:
                @display("p=150,0;i=abstract/multicast");
        }
        
        world: BaseWorldUtility {
            parameters:
                playgroundSizeX = veins_eval_by_version(veins_omnetpp_buildnum(), "playgroundSizeX", 1525, "parent.playgroundSizeX");
                playgroundSizeY = veins_eval_by_version(veins_omnetpp_buildnum(), "playgroundSizeY", 1525, "parent.playgroundSizeY");
                playgroundSizeZ = veins_eval_by_version(veins_omnetpp_buildnum(), "playgroundSizeZ", 1525, "parent.playgroundSizeZ");
                @display("p=413.472,329.37598;i=misc/globe");
        }
        
        manager: VeinsInetManager {
            parameters:
                @display("p=897.0239,455.52");
        }
        
        roadsCanvasVisualizer: RoadsCanvasVisualizer {
            @display("p=679.77594,329.37598");
        }
        
        generator[0]: TaskGenerator {
            @display("p=532.608,490.55997");
        }
        
        worker[0]: Worker {
            @display("p=469.53598,406.464");
        }
        
        beaconer[0]: Beaconer {
            @display("p=469.53598,406.464");
        }

    connections allowunconnected:
}
