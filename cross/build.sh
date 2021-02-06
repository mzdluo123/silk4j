cd native
mkdir cmake-build-debug
cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CACHEFILE_DIR=native/cmake-build-debug ..
make install -j8