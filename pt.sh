# build出错停止运行
set -e

INTEGATOR=pt
NUM_SAMPLE=5
ACCELERATOR=kdtree


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

time bin/PA1 testcases/green_rabit.txt output/green_rabit_pt.bmp $ACCELERATOR $NUM_SAMPLE $INTEGATOR >> test.log