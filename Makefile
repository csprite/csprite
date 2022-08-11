all:
	scons -j 4

release:
	scons -j 4 mode=release

clean:
	scons -c
