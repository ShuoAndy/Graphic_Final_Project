INTEGATOR=pt
NUM_SAMPLE=20
ACCELERATOR=kdtree


# If project not ready, generate cmake file.
if [[ ! -d build ]]; then
    echo "good"
else
    rm -rf build
fi
cmake -B build
cmake --build build

mkdir -p output


time bin/PA1 testcases/random_scene.txt output/random_scene.bmp $ACCELERATOR $NUM_SAMPLE $INTEGATOR >> test.log