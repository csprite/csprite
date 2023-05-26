GENERATOR=Ninja
BUILD_DIR=build/
BUILD_TYPE=Debug
NUM_JOBS=2

all:
	@cmake -S ./ -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -G "$(GENERATOR)" -L
	@cmake --build $(BUILD_DIR) --config=$(BUILD_TYPE) --parallel $(NUM_JOBS)

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
