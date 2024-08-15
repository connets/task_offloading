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

import task_offloading.app.nodes.WorkerVehicle;
import task_offloading.app.nodes.TaskGeneratorVehicle;
import task_offloading.app.nodes.BeaconerVehicle;
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
            @display("p=2831.232,868.99194");
        }

        loadBalancingAlgorithm: <default("FIFO")> like ISorting {
            @display("i=block/cogwheel;p=1352.544,2004.2878");
        }

        obstacles: ObstacleControl {
            @display("p=1303.4879,1478.688");
        }

        annotations: AnnotationManager {
            @display("p=2193.504,2095.3918");
        }

        connectionManager: ConnectionManager {
            parameters:
                @display("p=1948.2239,1576.7999;i=abstract/multicast");
        }

        world: BaseWorldUtility {
            parameters:
                playgroundSizeX = veins_eval_by_version(veins_omnetpp_buildnum(), "playgroundSizeX", 1525, "parent.playgroundSizeX");
                playgroundSizeY = veins_eval_by_version(veins_omnetpp_buildnum(), "playgroundSizeY", 1525, "parent.playgroundSizeY");
                playgroundSizeZ = veins_eval_by_version(veins_omnetpp_buildnum(), "playgroundSizeZ", 1525, "parent.playgroundSizeZ");
                @display("p=1681.9199,1114.272;i=misc/globe");
        }

        manager: VeinsInetManager {
            parameters:
                @display("p=672.76794,1478.688");
        }

        roadsCanvasVisualizer: RoadsCanvasVisualizer {
            @display("p=2067.3599,490.55997");
        }

        generator[0]: TaskGeneratorVehicle {
            @display("p=1737.9839,2726.1118");
        }

        worker[0]: WorkerVehicle {
            @display("p=1149.3119,2726.1118");
        }

        beaconer[0]: BeaconerVehicle {
            @display("p=469.53598,2726.1118");
        }

    connections allowunconnected:
}
