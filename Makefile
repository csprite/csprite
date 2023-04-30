all:
	@cmake -B build
	@cmake --build build

run: all
	@./build/csprite

