#!/bin/bash

set -e

git clone https://github.com/libsdl-org/SDL -b release-2.24.0
cd SDL
mkdir build
cd build
../configure
make -j4
sudo make install
