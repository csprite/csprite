#!/bin/bash

set -e

rm -f include/FontMontserrat_Bold.h
clang++ lib/binary_to_compressed_c.cpp -o bin2CompC
./bin2CompC ./assets/fonts/Montserrat-Bold.ttf Montserrat_Bold > ./include/FontMontserrat_Bold.h
