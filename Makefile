MajVer:=0
MinVer:=1
PatVer:=0

all:
	MajVer=$(MajVer) MinVer=$(MinVer) PatVer=$(PatVer) scons -j 4

release:
	MajVer=$(MajVer) MinVer=$(MinVer) PatVer=$(PatVer) scons -j 4 mode=release

version:
	@echo $(MajVer).$(MinVer).$(PatVer)

appimage:
	CSPRITE_VERSION=$(MajVer).$(MinVer).$(PatVer) appimage-builder --skip-test --recipe=AppImage-Builder.yml

clean:
	scons -c
	$(RM) -r data/icon.ico data/icons src/assets/*.inl tools/font2inl.out windows.o

gen-assets:
	python3 tools/create_icons.py
	python3 tools/create_assets.py

# For Windows RC
gen-rc:
	python3 tools/create_rc.py --arch=x86_64 --majver=$(MajVer) --minver=$(MinVer) --patver=$(PatVer)
