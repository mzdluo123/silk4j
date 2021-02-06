cd native
mkdir cmake-build-debug
cd cmake-build-debug
rm -rf CMakeCache.txt
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CACHEFILE_DIR=$PWD ..
make install -j8