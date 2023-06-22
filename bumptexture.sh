INTEGATOR=pt
NUM_SAMPLE=500
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

time bin/PA1 testcases/bumptexture.txt output/bumptexture.bmp $ACCELERATOR $NUM_SAMPLE $INTEGATOR  >> test.log