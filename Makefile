NUM_JOBS=2
GENERATOR=Ninja
BUILD_DIR=build/
BUILD_TYPE=Debug

# These Flags are passed to Cmake When Generating Build Files
CMAKE_GEN_FLAGS=

# These Flags are passed to Cmake When Building The Project
CMAKE_BUILD_FLAGS=

all:
	@cmake -L -S ./ -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -G "$(GENERATOR)" $(CMAKE_GEN_FLAGS)
	@cmake --build $(BUILD_DIR) --config=$(BUILD_TYPE) --parallel $(NUM_JOBS) $(CMAKE_BUILD_FLAGS)

clean:
	@$(RM) -r $(BUILD_DIR)

run: all
	@./build/csprite

# make gen-assets PYTHON=python3
gen-assets:
	$(eval PYTHON := $(if $(PYTHON),$(PYTHON),python3))
	@$(PYTHON) tools/create_icons.py
	@echo - Icons generated
	@$(PYTHON) tools/create_assets.py --cxx=$(CXX)
	@echo - Assets generated
