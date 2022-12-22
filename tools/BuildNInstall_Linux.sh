#!/bin/bash

set -e

rm -rf SDL/
git clone https://github.com/libsdl-org/SDL -b release-2.26.1
cd SDL/
mkdir build
cd build/
../configure --enable-timers --enable-video --enable-events --disable-render --disable-render-d3d --disable-video-vulkan --disable-video-opengles1 --disable-video-dummy  --disable-audio --disable-joystick --disable-haptic --disable-joystick-virtual --disable-hidapi --disable-sensor --disable-power --disable-filesystem --disable-file --disable-misc --disable-loadso --disable-cpuinfo --disable-oss --disable-alsa --disable-alsatest --disable-alsa-shared --disable-jack --disable-jack-shared --disable-esd --disable-esdtest --disable-esd-shared --disable-pipewire --disable-pipewire-shared --disable-pulseaudio --disable-pulseaudio-shared --disable-arts --disable-arts-shared --disable-nas --disable-nas-shared --disable-sndio --disable-sndio-shared --disable-fusionsound --disable-fusionsound-shared --disable-diskaudio --disable-dummyaudio --disable-libsamplerate --disable-libsamplerate-shared --disable-joystick-mfi --disable-hidapi-joystick --disable-hidapi-libusb
make -j4
echo "libSDL2.a Contents:"
ar -t ./build/.libs/libSDL2.a

# On Ubuntu 18.04 VM Sudo Isn't Installed.
if ! command -v sudo &> /dev/null; then
	echo "Sudo Not Found, Installing Without Sudo"
	make install
else
	sudo make install
fi
