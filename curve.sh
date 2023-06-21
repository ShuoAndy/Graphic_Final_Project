# build出错停止运行
set -e

INTEGATOR=pt
NUM_SAMPLE=2
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

time bin/PA1 testcases/curve.txt output/curve.bmp $ACCELERATOR $NUM_SAMPLE $INTEGATOR  >> test.log