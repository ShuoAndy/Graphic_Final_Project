#!/usr/bin/env bash

INTEGATOR=sppm
ACCELERATOR=bvh
STEP=10
NUM_SAMPLE=5000
NUM_PHOTON=1000000
USEV2=1

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

# Run all testcases. 
# You can comment some lines to disable the run of specific examples.
mkdir -p output
#bin/PA1 testcases/random_scene.txt output/random_scene.bmp
#time bin/PA1 testcases/con_box_test.txt output/con_box_bvh_test.bmp bvh 2000 >> test.log
#time bin/PA1 testcases/con_box.txt output/con_box_kdtree.bmp kdtree 20 >> test.log
#time bin/PA1 testcases/con_box.txt output/con_box_seq.bmp seq 20 >> test.log
#bin/PA1 testcases/texture.txt output/texture.bmp
#time bin/PA1 testcases/smallpt.txt output/smallpt_sppm $ACCELERATOR $NUM_SAMPLE $INTEGATOR >> test.log
mkdir -p output/smallpt_sppm_rec
time bin/PA1 testcases/small_pt_rec.txt output/smallpt_sppm_rec $ACCELERATOR $NUM_SAMPLE $INTEGATOR $USEV2 $STEP $NUM_PHOTON >> test.log
#mkdir -p output/sponza_sppm
#time bin/PA1 sponza/s.txt output/sponza_sppm $ACCELERATOR $NUM_SAMPLE $INTEGATOR $STEP $NUM_PHOTON >> test.log
#mkdir -p output/con_box_test_sppm_v2
#time bin/PA1 testcases/con_box_test.txt output/con_box_test_sppm_v2 $ACCELERATOR $NUM_SAMPLE $INTEGATOR $USEV2 $STEP $NUM_PHOTON >> test.log
#bin/PA1 testcases/test_bump.txt output/test_bump.bmp
#time bin/PA1 testcases/small_rabit.txt output/small_rabit_bvh.bmp bvh 200 >> test.log