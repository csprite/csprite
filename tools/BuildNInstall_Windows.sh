#!/bin/bash

set -e

if [ ! -z $1 ]; then
    echo "Msys Environment: $1"
else
    echo "Error, No Msys Environment Specified."
	echo "Usage: $0 <msys-environment>"
	echo "Example: $0 mingw64"
    exit 1
fi
if ! command -v cmake &> /dev/null; then
	echo "CMake Not Found, Please Install CMake!"
	exit 1
fi

rm -rf SDL/
git clone https://github.com/libsdl-org/SDL -b release-2.26.2
cd SDL/
mkdir build
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Release -DSDL2_DISABLE_SDL2MAIN=OFF -DSDL_VIDEO=ON -DSDL_RENDER=ON -DSDL_DIRECTX=ON -DSDL_RENDER_D3D=ON -DSDL_VULKAN=ON -DSDL_XINPUT=ON -DSDL_EVENTS=ON -DSDL_LOADSO=ON -DSDL_THREADS=ON -DSDL_TIMERS=ON -DSDL_ATOMIC=OFF -DSDL_AUDIO=OFF -DSDL_JOYSTICK=OFF -DSDL_HAPTIC=OFF -DSDL_HIDAPI=OFF -DSDL_POWER=OFF -DSDL_FILE=OFF -DSDL_CPUINFO=OFF -DSDL_FILESYSTEM=OFF -DSDL_SENSOR=OFF -DSDL_LOCALE=OFF -DSDL_MISC=OFF -DSDL_DISKAUDIO=OFF -DSDL_DUMMYAUDIO=OFF -DSDL_DUMMYVIDEO=OFF -DSDL_HIDAPI=OFF -DSDL_HIDAPI_JOYSTICK=OFF -DSDL_VIRTUAL_JOYSTICK=OFF -DSDL_WASAPI=OFF
cmake --build . --config Release --parallel
cmake --install . --config Release --prefix=/$1/
