INTEGATOR=pt
NUM_SAMPLE=2000
ACCELERATOR=bvh
USEV2=0

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

time bin/PA1 testcases/cornell_box.txt output/cornell_box.bmp $ACCELERATOR $NUM_SAMPLE $INTEGATOR $USEV2 >> test.log