cd build/Debug
cmake -DCMAKE_BUILD_TYPE=DEBUG ../../
make -j
cd ../../
./build/DEBUG/preview "$@"
