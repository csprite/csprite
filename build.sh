#!/bin/sh

set -eu

CC=${CC:-gcc}
CXX=${CXX:-g++}
LD=${LD:-g++}
BUILD="build"
BIN="$BUILD/csprite"
FLAGS='-march=native -Wall -Wextra -pedantic -Isrc/ -Ivendor/glad/ -Ivendor/log.c/include/ -Ivendor/stb/include -ffast-math -D_DEFAULT_SOURCE=1 -DCIMGUI_NO_EXPORT=1'
CFLAGS='-std=c99 -fvisibility=hidden -DCIMGUI_USE_OPENGL3=1 -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS=1 -DLOG_USE_COLOR=1'
CXXFLAGS='-fvisibility=hidden'
LFLAGS='-fvisibility=hidden'
CMD=${1:-}
KERNEL=$(uname -s)
MAYBE_WAIT=""

if [ "$KERNEL" = "Linux" ]; then
	FLAGS="$FLAGS -DTARGET_LINUX=1 -DCIMGUI_USE_GLFW=1"
	LFLAGS="$LFLAGS -lglfw -lX11"
elif [ "$KERNEL" = "Windows_NT" ] || [ "$(uname -o)" = "Cygwin" ]; then
	FLAGS="$FLAGS -DTARGET_WINDOWS=1 -DWIN32_LEAN_AND_MEAN=1 -DWINVER=0x0601 -D_WIN32_WINNT=0x0601 -DCIMGUI_USE_WIN32=1"
	LFLAGS="$LFLAGS -lgdi32 -lopengl32 -lcomdlg32"
	BIN="$BIN.exe"
	# On BusyBox.exe, It seems to run out of memory if too many commands are spawned.
	# So we just wait it out.
	MAYBE_WAIT="wait"
fi

SOURCES="src/app/main.c src/app/gui.c src/app/render.c src/app/editor.c src/os/os.c src/os/gfx.c src/base/arena.c src/base/string.c src/bitmap/bitmap.c src/assets/assets.c src/gfx/gfx.c vendor/glad/impl.c vendor/log.c/src/log.c vendor/stb/impl.c"
SOURCES_CPP="src/cimgui/impl.cpp"
OBJECTS="$(echo "$SOURCES" | sed "s|\([^ ]*\)\.c|$BUILD/\1.c.o|g") $(echo "$SOURCES_CPP" | sed "s|\([^ ]*\)\.cpp|$BUILD/\1.cpp.o|g")"

if [ "$CMD" = "clean" ]; then
	rm -rf $BIN $BUILD src/assets/assets.inl
	exit 0
elif [ "$CMD" = "bear" ]; then
	bear --append --output "$BUILD/compile_commands.json" -- "$0" # github.com/rizsotto/Bear
	exit 0
elif [ "$CMD" = "assets" ]; then
	if [ -x "$(command -v python3)" ]; then
		PYTHON=python3
	elif [ -x "$(command -v python)" ]; then
		PYTHON=python
	else
		echo "Python not found!"
		exit 1
	fi
	echo "Produce src/assets/assets.inl"
	$PYTHON tools/create_icons.py
	$PYTHON tools/create_assets.py --cxx="$CXX"
	exit 0
elif [ "$CMD" = "release" ]; then
	# TODO(pegvin) - Look into https://stackoverflow.com/q/6687630/14516016
	# in detail & figure out a way to strip all the unused functions, Since
	# we won't be used most of the ImGui's functions anyways.
	FLAGS="$FLAGS -O3 -fdata-sections -ffunction-sections -DBUILD_RELEASE=1"
	LFLAGS="$LFLAGS -Wl,--gc-sections"
elif [ "$CMD" = "" ]; then
	FLAGS="$FLAGS -O0 -g3 -fsanitize=address,undefined -DBUILD_DEBUG=1 -DBUILD_HAS_ASAN=1"
	LFLAGS="$LFLAGS -fsanitize=address,undefined"
elif [ "$CMD" ]; then
	echo "Invalid command '$CMD', Available commands are: clean/bear/assets/release or none to just build in debug mode."
	exit 1
fi

if ! [ -x "$(command -v ccache)" ]; then CCACHE=""; else CCACHE="ccache"; fi
if [ -x "$(command -v mold)" ]; then LFLAGS="$LFLAGS -fuse-ld=mold"; fi
if [ "$CXX" = "clang++" ] && [ "$KERNEL" = "Linux" ]; then FLAGS="$FLAGS -fopenmp=libomp"; LFLAGS="$LFLAGS -fopenmp=libomp"; else FLAGS="$FLAGS -fopenmp"; LFLAGS="$LFLAGS -fopenmp"; fi

export CCACHE_DIR="$BUILD/.ccache"
mkdir -p $BUILD $CCACHE_DIR

echo "$SOURCES $SOURCES_CPP 0" | tr ' ' '\n' | while read -r source; do
	if [ "$source" = "0" ]; then wait; exit 0; fi
	echo "Compiling $source"
	mkdir -p "$(dirname "$BUILD/$source.o")"

	if [ "$(echo "$source" | sed 's/^.*\.//')" = "c" ]; then
		$CCACHE $CC $FLAGS $CFLAGS -c "$source" -o "$BUILD/$source.o" &
	else
		$CCACHE $CXX $FLAGS $CXXFLAGS -c "$source" -o "$BUILD/$source.o" &
	fi
	$MAYBE_WAIT
done

echo "  Linking $BIN"
$CCACHE $LD $OBJECTS $LFLAGS -o "$BIN"
