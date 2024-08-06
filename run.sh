if [ ! -d "./build" ]; then
    mkdir build
fi

cmake -B./build/ -S./ -DCMAKE_GENERATOR:INTERNAL=Ninja
if [ $? -ne 0 ]; then
    echo "cmake failed"
    exit 1
fi

cmake --build ./build/
if [ $? -ne 0 ]; then
    echo "build failed"
    exit 1
fi

./build/Teteko $@
if [ $? -ne 0 ]; then
    echo "run failed"
    exit 1
fi
