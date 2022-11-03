MajVer:=1
MinVer:=0
PatVer:=0
Stable:=0

CC:=gcc
CXX:=g++
CFLAGS:=-std=c99
CXXFLAGS:=-std=c++17
LFLAGS:=

CCFLAGS:=-Isrc/ -Iinclude/ -Ilib/ -Ilib/tfd/src -Wall
CCFLAGS+=-DCS_VERSION_MAJOR=$(MajVer) -DCS_VERSION_MINOR=$(MinVer) -DCS_VERSION_PATCH=$(PatVer) -DCS_BUILD_STABLE=$(Stable)
CCFLAGS+=-DLOG_USE_COLOR

bin:=csprite
SRCS_C:=$(wildcard src/*.c) $(wildcard lib/imgui/*.c) $(wildcard lib/log/*.c) $(wildcard lib/ini/*.c) $(wildcard lib/tfd/src/*.c) $(wildcard lib/downloader/*.c)
SRCS_CPP+=$(wildcard src/*.cpp) $(wildcard lib/imgui/*.cpp) $(wildcard lib/log/*.cpp) $(wildcard lib/ini/*.cpp) $(wildcard lib/tfd/src/*.cpp) $(wildcard lib/downloader/*.cpp)
OBJS_C:=$(SRCS_C:.c=.o)
OBJS_CPP+=$(SRCS_CPP:.cpp=.o)

ifeq ($(Stable),0)
	CCFLAGS+=-O0 -g -Wno-unused-function
	CCFLAGS+=$(addprefix -D, IS_DEBUG SHOW_FRAME_TIME SHOW_HISTORY_LOGS)
else
	CCFLAGS+=-Os -DENABLE_WIN_ICON
endif

ifeq ($(OS),Windows_NT)
	CCFLAGS+=-DSDL_MAIN_HANDLED
	LFLAGS+=$(addprefix -l,SDL2main SDL2 mingw32 opengl32 comdlg32 imagehlp dinput8 dxguid dxerr8 user32 gdi32 winmm imm32 ole32 oleaut32 shell32 version uuid setupapi)
	LFLAGS+=-mwindows --static
	bin=csprite.exe
else
	UNAME_S:=$(shell uname -s)
	_libs:=SDL2 m

	# On POSX Use Address Sanitizers in Debug Mode
	ifeq ($(Stable),0)
		ifeq ($(CXX),g++)
			CCFLAGS+=-fsanitize=address -fsanitize=undefined
			LFLAGS+=-fsanitize=address -fsanitize=undefined -lasan -lubsan
		endif
	endif

	ifeq ($(UNAME_S),Linux)
		_libs+=dl
	endif
	ifeq ($(UNAME_S),Darwin)
		_libs+=objc
		LFLAGS+=$(addprefix -framework , OpenGL Cocoa)
	endif

	LFLAGS+=$(addprefix -l,$(_libs))
endif

all: $(bin)

%.o: %.c
	$(CC) $(CFLAGS) $(CCFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(CCFLAGS) -c $< -o $@

$(bin): $(OBJS_C) $(OBJS_CPP)
	$(CXX) $(OBJS_C) $(OBJS_CPP) $(LFLAGS) -o $@

.PHONY: run
.PHONY: clean

run: $(all)
	./$(bin)

clean:
	$(RM) $(bin) $(OBJS_C) $(OBJS_CPP) data/icon.ico src/assets/*.inl tools/font2inl.out windows.o
	$(RM) -r data/icons

targz:
	bash tools/build_targz.sh $(MajVer).$(MinVer).$(PatVer)

version:
	@echo $(MajVer).$(MinVer).$(PatVer)

appimage:
	CSPRITE_VERSION=$(MajVer).$(MinVer).$(PatVer) appimage-builder --skip-test --recipe=AppImage-Builder.yml

gen-assets:
	python3 tools/create_icons.py
	python3 tools/create_assets.py

# For Windows RC
gen-rc:
	python3 tools/create_rc.py --arch=x86_64 --majver=$(MajVer) --minver=$(MinVer) --patver=$(PatVer)
