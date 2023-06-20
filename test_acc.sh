INTEGATOR=pt
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

mkdir -p output

time bin/PA1 testcases/green_rabit.txt output/green_rabit_bvh.bmp bvh $NUM_SAMPLE $INTEGATOR  >> test.log
time bin/PA1 testcases/green_rabit.txt output/green_rabit_kdtree.bmp kdtree $NUM_SAMPLE $INTEGATOR  >> test.log
time bin/PA1 testcases/green_rabit.txt output/green_rabit_brute_force.bmp seq $NUM_SAMPLE $INTEGATOR  >> test.log

