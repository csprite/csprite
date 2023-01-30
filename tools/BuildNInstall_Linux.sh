#!/bin/bash

set -e

rm -rf SDL/
git clone https://github.com/libsdl-org/SDL -b release-2.26.2
cd SDL/
mkdir build
cd build/
../configure --enable-timers --enable-video --enable-render --enable-events --disable-atomic --disable-locale --disable-audio --disable-joystick --disable-haptic --disable-hidapi --disable-sensor --disable-power --disable-filesystem --disable-file --disable-misc --disable-cpuinfo --disable-hidapi-joystick --disable-hidapi-libusb --disable-oss --disable-alsa --disable-jack --disable-esd --disable-esdtest --disable-pipewire --disable-pulseaudio --disable-arts --disable-nas --disable-sndio --disable-fusionsound --disable-diskaudio --disable-dummyaudio --disable-libsamplerate --disable-joystick-mfi
make -j4

# On Ubuntu 18.04 VM Sudo Isn't Installed.
if ! command -v sudo &> /dev/null; then
	echo "Sudo Not Found, Installing Without Sudo"
	make install
else
	sudo make install
fi
