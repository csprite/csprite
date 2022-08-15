MajVer:=0
MinVer:=1
PatVer:=0

all:
	MajVer=$(MajVer) MinVer=$(MinVer) PatVer=$(PatVer) scons -j 4

release:
	MajVer=$(MajVer) MinVer=$(MinVer) PatVer=$(PatVer) scons -j 4 mode=release

clean:
	scons -c
	$(RM) -r data/icon.ico data/icons src/assets/*.inl tools/font2inl.out windows.o

generate:
	tools/create_icons.py
	tools/create_assets.py
	tools/create_rc.py --arch=x86_64 --majver=$(MajVer) --minver=$(MinVer) --patver=$(PatVer)
