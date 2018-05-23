git submodule init
git submodule update
mkdir build install
git apply --stat python2.7.patch
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../install
make install -j 4


