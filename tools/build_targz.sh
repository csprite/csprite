#!/bin/bash

set -e

if [ -z "$1" ]; then
	echo "Usage: $0 <version>"
else
	rm -rf "csprite-$1"
	mkdir "csprite-$1"
	cd "csprite-$1"
	mkdir -p {bin/,share/icons/hicolor/,share/applications}
	cp ../data/csprite.desktop share/applications
	cd share/icons/hicolor/
	mkdir -p {16x16/apps,32x32/apps,48x48/apps,64x64/apps,128x128/apps,256x256/apps,512x512/apps}
	cp ../../../../data/icons/icon-16.png 16x16/apps/csprite.png
	cp ../../../../data/icons/icon-32.png 32x32/apps/csprite.png
	cp ../../../../data/icons/icon-48.png 48x48/apps/csprite.png
	cp ../../../../data/icons/icon-64.png 64x64/apps/csprite.png
	cp ../../../../data/icons/icon-128.png 128x128/apps/csprite.png
	cp ../../../../data/icons/icon-256.png 256x256/apps/csprite.png
	cp ../../../../data/icons/icon-512.png 512x512/apps/csprite.png
	cd ../../../
	cp ../csprite bin/
	cd ..
	tar czvf csprite-$1.tar.gz "csprite-$1"
	rm -rf "csprite-$1"
fi
