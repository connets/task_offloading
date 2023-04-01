#!/bin/bash

source /omnetpp-6.0.1/setenv

# Activate TRACI server
veins-veins-5.2/bin/veins_launchd -v &

RES_DIR=/app/results

# Launch simulations
opp_runall -j8 -b1 task_offloading/out/gcc-release/src/task_offloading -m -u Cmdenv --cmdenv-stop-batch-on-error=false \
	-n task_offloading/simulations:task_offloading/src:inet/src:veins-veins-5.2/src/veins \
	-l inet/src/INET -l veins-veins-5.2/src/veins \
	task_offloading/simulations/v5_0_Final/omnetpp.ini -c Default --result-dir=$RES_DIR
