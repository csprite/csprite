BUILD_TYPE=Debug

all:
	@cmake -S ./ -B build -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -G "Ninja" -L
	@cmake --build build --parallel 3

run: all
	@./build/csprite

# make gen-assets PYTHON=python3
gen-assets:
	$(eval PYTHON := $(if $(PYTHON),$(PYTHON),python3))
	@$(PYTHON) tools/create_icons.py
	@echo - Icons generated
	@$(PYTHON) tools/create_assets.py --cxx=$(CXX)
	@echo - Assets generated
