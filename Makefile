all:
	scons -j 4

release:
	scons -j 4 mode=release

clean:
	scons -c
	$(RM) -r data/icon.ico data/icons src/assets/*.inl tools/font2inl.out

generate:
	tools/create_icons.py
	tools/create_assets.py
