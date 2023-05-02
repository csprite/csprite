BUILD_TYPE=Debug

all:
	@cmake -S ./ -B build -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -G "Ninja" -L
	@cmake --build build --parallel 3

run: all
	@./build/csprite
