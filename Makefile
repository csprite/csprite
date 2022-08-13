all:
	scons -j 4

release:
	MajVer=0 MinVer=1 PatVer=0 scons -j 4 mode=release

clean:
	scons -c
	$(RM) -r data/icon.ico data/icons src/assets/*.inl tools/font2inl.out windows.o

generate:
	tools/create_icons.py
	tools/create_assets.py
