#!/bin/bash

source /omnetpp-6.0.1/setenv

# Activate TRACI server
veins-veins-5.2/bin/veins_launchd -v &

RES_DIR=/app/results

# Launch simulations
opp_runall -j8 -b1 task_offloading/out/gcc-release/src/task_offloading -m -u Cmdenv --cmdenv-stop-batch-on-error=false \
	-n task_offloading/simulations:task_offloading/src:inet/src:veins-veins-5.2/src/veins:veins-veins-5.2/subprojects/veins_inet/src/veins_inet \
	-l inet/src/INET -l veins-veins-5.2/src/veins -l veins-veins-5.2/subprojects/veins_inet/src/veins_inet \
	task_offloading/simulations/task_offloading/omnetpp.ini -c Default --result-dir=$RES_DIR
