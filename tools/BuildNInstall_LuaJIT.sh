#!/bin/bash

set -e

if [ ! -z $1 ]; then
    echo "Msys Environment: $1"
else
    echo "Warning, No prefix DESTDIR was specified"
    echo "if on msys2 specify the Environment name"
	echo "Example: $0 mingw64"
fi

git clone https://github.com/LuaJIT/LuaJIT -b v2.0.5 luajit
cd luajit
make -j4 DESTDIR=$1

if ! command -v sudo &> /dev/null; then # on msys2 no sudo in most cases
	echo "Sudo Not Found, Installing Without Sudo"
	make install
else
	sudo make install
fi

