#!/usr/bin/env bash

INTEGATOR=sppm
ACCELERATOR=bvh
STEP=5
NUM_SAMPLE=5000
NUM_PHOTON=1000000


#rm -r ./build

# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    mkdir -p build
    cd build
    cmake ..
    cd ..
fi

# Build project.
cd build
make -j
cd ..

mkdir -p output
mkdir -p output/sponza_sppm
time bin/PA1 sponza/sppm.txt output/sponza_sppm $ACCELERATOR $NUM_SAMPLE $INTEGATOR $STEP $NUM_PHOTON >> test.log