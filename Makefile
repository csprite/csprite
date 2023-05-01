BUILD_TYPE=Debug

all:
	@cmake -S ./ -B build -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -G "Ninja"
	@cmake --build build

run: all
	@./build/csprite
