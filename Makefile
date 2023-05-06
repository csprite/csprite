BUILD_TYPE=Debug

all:
	@cmake -S ./ -B build -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -G "Ninja" -L
	@cmake --build build --parallel 3

run: all
	@./build/csprite

# make gen-rc ARCH=x86_64(or i686) PYTHON=python3
gen-rc:
	$(eval ARCH := $(if $(ARCH),$(ARCH),x86_64))
	$(eval PYTHON := $(if $(PYTHON),$(PYTHON),python3))
	@$(PYTHON) tools/create_rc.py --arch=$(ARCH) --majver=$(MajVer) --minver=$(MinVer) --patver=$(PatVer)
	@echo - RC generated

# make gen-assets PYTHON=python3
gen-assets:
	$(eval PYTHON := $(if $(PYTHON),$(PYTHON),python3))
	@$(PYTHON) tools/create_icons.py
	@echo - Icons generated
	@$(PYTHON) tools/create_assets.py --cxx=$(CXX)
	@echo - Assets generated

# make appimage
appimage:
	@CSPRITE_VERSION=$(MajVer).$(MinVer).$(PatVer) appimage-builder --skip-test --recipe=AppImage-Builder.yml
	@echo - AppImage created
