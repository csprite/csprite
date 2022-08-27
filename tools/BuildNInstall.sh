#!/bin/bash

set -e

rm -rf SDL/
git clone https://github.com/libsdl-org/SDL -b release-2.24.0
cd SDL/
mkdir build
cd build/
../configure
make -j4

# On Ubuntu 18.04 VM Sudo Isn't Installed.
if ! command -v sudo &> /dev/null; then
	echo "Sudo Not Found, Installing Without Sudo"
	make install
else
	sudo make install
fi
