#!/bin/bash

set -e

rm -f include/FontMontserrat_Bold.h
clang++ lib/font2inl.cpp -o font2inl
clang lib/image2inl.c -lm -o image2inl
./font2inl ./assets/fonts/Montserrat-Bold.ttf Montserrat_Bold ./include/FontMontserrat_Bold.inl
./image2inl ./assets/icon-16.png ProgramIcon16 ./include/ProgramIcon16.inl
./image2inl ./assets/icon-32.png ProgramIcon32 ./include/ProgramIcon32.inl
./image2inl ./assets/icon-48.png ProgramIcon48 ./include/ProgramIcon48.inl
