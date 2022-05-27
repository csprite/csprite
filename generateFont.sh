#!/bin/bash

set -e

rm -f include/FontMontserrat_Bold.h
clang++ lib/binary2header.cpp -o bin2header
./bin2header ./assets/fonts/Montserrat-Bold.ttf Montserrat_Bold > ./include/FontMontserrat_Bold.h
