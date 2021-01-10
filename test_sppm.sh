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
mkdir -p output/green_rabit_sppm
time bin/PA1 testcases/green_rabit.txt output/green_rabit_sppm $ACCELERATOR 3000 sppm $USEV2 100 1000000 >> test.log
time bin/PA1 testcases/green_rabit.txt output/green_rabit_pt.bmp $ACCELERATOR 2000 pt $USEV2 >> test.log