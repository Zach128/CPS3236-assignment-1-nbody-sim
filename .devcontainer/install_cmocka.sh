#!/bin/sh

curl https://cmocka.org/files/1.1/cmocka-1.1.5.tar.xz | tar -xJ
mkdir cmocka-1.1.5/build
cd cmocka-1.1.5/build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Debug ..
make
make install
cd ../../
rm -rf cmocka-1.1.5
