#!/usr/bin/env bash

#rm -r ./build
INTEGATOR=pt
ACCELERATOR=bvh
NUM_SAMPLE=20

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
#bin/PA1 testcases/scene01_basic.txt output/scene01.bmp
#bin/PA1 testcases/scene02_cube.txt output/scene02.bmp
#bin/PA1 testcases/scene03_sphere.txt output/scene03.bmp
#bin/PA1 testcases/scene04_axes.txt output/scene04.bmp
#bin/PA1 testcases/scene05_bunny_200.txt output/scene05.bmp
#bin/PA1 testcases/scene06_bunny_1k.txt output/scene06.bmp
#bin/PA1 testcases/scene07_shine.txt output/scene07.bmp
#bin/PA1 testcases/random_scene.txt output/random_scene.bmp
#time bin/PA1 testcases/con_box_test.txt output/con_box_bvh_test.bmp $ACCELERATOR $NUM_SAMPLE $INTEGATOR >> test.log
time bin/PA1 testcases/test_curve.txt output/curve_pt.bmp $ACCELERATOR $NUM_SAMPLE $INTEGATOR >> test.log
#time bin/PA1 testcases/con_box.txt output/con_box_kdtree.bmp kdtree 20 >> test.log
#time bin/PA1 testcases/con_box.txt output/con_box_seq.bmp seq 20 >> test.log
#bin/PA1 testcases/texture.txt output/texture.bmp
#time bin/PA1 testcases/small_pt_rec.txt output/smallpt_pt_rec.bmp $ACCELERATOR $NUM_SAMPLE $INTEGATOR >> test.log
#bin/PA1 testcases/test_bump.txt output/test_bump.bmp
#time bin/PA1 testcases/small_rabit.txt output/small_rabit_bvh.bmp bvh 200 >> test.log