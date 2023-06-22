# build出错停止运行
set -e

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
mkdir -p output/green_rabit_sppm
time bin/PA1 testcases/rabit.txt output/rabit_sppm $ACCELERATOR 3000 sppm  100 1000000 >> test.log
