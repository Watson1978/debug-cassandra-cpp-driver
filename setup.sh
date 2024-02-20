wget https://github.com/datastax/cpp-driver/archive/2.17.1.tar.gz
tar xvzf 2.17.1.tar.gz
cd cpp-driver-2.17.1
patch CMakeLists.txt ../cpp-driver.patch

mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=../..
make -j install