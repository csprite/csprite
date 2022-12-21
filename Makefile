MajVer:=1
MinVer:=0
PatVer:=0
Stable:=0
Arch:=x86_64 # Used By gen-rc task
Windres_Target:=pe-x86-64 # or pe-i686 for 32 bit system

CXX:=g++
CC:=gcc
STD:=c99
CXX_STD:=c++17
CFLAGS:=
CCFLAGS+=-Iinclude/ -Ilibs/imgui/ -Ilibs/ -Wall -MMD -MP -DCS_VERSION_MAJOR=$(MajVer) -DCS_VERSION_MINOR=$(MinVer) -DCS_VERSION_PATCH=$(PatVer) -DCS_BUILD_STABLE=$(Stable) -DSDL_MAIN_HANDLED=1 -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1 -DLOG_USE_COLOR=1
LFLAGS:=

SRCS_C:=$(wildcard src/*.c) $(wildcard libs/ini/*.c) $(wildcard libs/log/*.c) $(wildcard libs/tfd/*.c)
SRCS_CPP:=$(wildcard src/*.cpp) $(wildcard libs/imgui/*.cpp)
OBJS_C:=$(SRCS_C:.c=.o)
OBJS_CPP:=$(SRCS_CPP:.cpp=.o)
DEPENDS:=$(patsubst %.c,%.d,$(SRCS_C)) $(patsubst %.cpp,%.d,$(SRCS_CPP))
bin:=csprite

ifeq ($(Stable),0)
	CCFLAGS+=-O0 -g
else
	CCFLAGS+=-O2
endif

ifeq ($(OS),Windows_NT)
	LFLAGS+=$(addprefix -l,SDL2main SDL2 opengl32 winmm imm32 ole32 oleaut32 shell32 version uuid setupapi)
	LFLAGS+=-mwindows --static
	SRCS_C+=windows.rc
	OBJS_C+=windows.o
	bin=csprite.exe
else
	UNAME_S:=$(shell uname -s)
	_libs:=SDL2 m

	ifeq ($(UNAME_S),Linux)
		LFLAGS+=--static
		_libs+=dl
		# On POSX Use Address Sanitizers in Debug Mode
		ifeq ($(CC),gcc)
			ifeq ($(Stable),0)
				CCFLAGS+=-fsanitize=address -fsanitize=undefined
				LFLAGS+=-fsanitize=address -fsanitize=undefined -lasan -lubsan
			endif
		endif
	endif
	ifeq ($(UNAME_S),Darwin)
		LFLAGS+=$(addprefix -framework , OpenGL Cocoa)
	endif

	LFLAGS+=$(addprefix -l,$(_libs))
endif

# make all Windres_Target=pe-x86-64(or pe-i386, Windres_Target is only needed on windows builds, this also requires make gen-rc)
all: $(bin)

-include $(DEPENDS)

%.o: %.rc
	windres.exe -O COFF -F $(Windres_Target) -i $< -o $@

%.o: %.c
	$(CC) --std=$(STD) $(CFLAGS) $(CCFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) --std=$(CXX_STD) $(CXXFLAGS) $(CCFLAGS) -c $< -o $@

$(bin): $(OBJS_C) $(OBJS_CPP)
	$(CXX) --std=$(CXX_STD) $(OBJS_C) $(OBJS_CPP) $(LFLAGS) -o $@

.PHONY: run
.PHONY: clean

# make run
run: $(all)
	./$(bin)

# make clean
clean:
	$(RM) $(bin) $(OBJS_C) $(OBJS_CPP) $(DEPENDS) ./csprite.exe.manifest windows.rc tools/font2inl.out data/*.ico data/icons/*.png src/assets/*.inl

# make gen-rc Arch=x86_64(or i686)
gen-rc:
	python3 tools/create_rc.py --arch=$(Arch) --majver=$(MajVer) --minver=$(MinVer) --patver=$(PatVer)

# make gen-assets
gen-assets:
	python3 tools/create_icons.py
	python3 tools/create_assets.py

# make appimage
appimage:
	CSPRITE_VERSION=$(MajVer).$(MinVer).$(PatVer) appimage-builder --skip-test --recipe=AppImage-Builder.yml
