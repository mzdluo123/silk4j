cd native
mkdir cmake-build-debug
cd cmake-build-debug
rm -rf CMakeCache.txt
cmake -DCMAKE_BUILD_TYPE=Release ..
make install -j8