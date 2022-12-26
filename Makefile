MajVer:=1
MinVer:=0
PatVer:=0
Arch:=x86_64 # Used By gen-rc task

CXX:=g++
CC:=gcc
STD:=c99
CXX_STD:=c++17
CCFLAGS:=-Iinclude/ -Ilibs/imgui/ -Ilibs/ -Wall -MMD -MP -DCS_VERSION_MAJOR=$(MajVer) -DCS_VERSION_MINOR=$(MinVer) -DCS_VERSION_PATCH=$(PatVer) -DSDL_MAIN_HANDLED=1 -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1 -DLOG_USE_COLOR=1
CFLAGS:=
LFLAGS:=

SDL2_STATIC_LINK:=1
WINDRES_TARGET:=pe-x86-64 # pe-x86-64 for 64 bit system or pe-i686 for 32 bit system
BUILD_TARGET=debug

ifeq ($(BUILD_TARGET),debug)
	CCFLAGS+=-DCS_BUILD_STABLE=0
else
	ifeq ($(BUILD_TARGET),release)
		CCFLAGS+=-DCS_BUILD_STABLE=1
	else
$(error Invalid Build Target: "$(BUILD_TARGET)")
	endif
endif

SRCS_C:=$(wildcard src/*.c) $(wildcard libs/ini/*.c) $(wildcard libs/log/*.c) $(wildcard libs/tfd/*.c)
SRCS_CPP:=$(wildcard src/*.cpp) $(wildcard libs/imgui/*.cpp) $(wildcard libs/imgooeystyles/*.cpp)
OBJS_C:=$(SRCS_C:.c=.o)
OBJS_CPP:=$(SRCS_CPP:.cpp=.o)
DEPENDS:=$(patsubst %.c,%.d,$(SRCS_C)) $(patsubst %.cpp,%.d,$(SRCS_CPP))
bin:=csprite

ifeq ($(BUILD_TARGET),debug)
	CCFLAGS+=-O0 -g
else
	CCFLAGS+=-O2
endif

ifeq ($(OS),Windows_NT)
	ifeq ($(SDL2_STATIC_LINK),1)
		LFLAGS+=-static-libstdc++ -Wl,-Bstatic -lSDL2main -lSDL2 -Wl,-Bdynamic
	else
		LFLAGS+=-lSDL2main -lSDL2
	endif
	LFLAGS+=$(addprefix -l,opengl32 winmm gdi32 imm32 ole32 oleaut32 shell32 version uuid setupapi)
	ifeq ($(BUILD_TARGET),debug)
		LFLAGS+=-mconsole
	else
		LFLAGS+=-mwindows
	endif
	SRCS_C+=windows.rc
	OBJS_C+=windows.o
	bin=csprite.exe
else
	UNAME_S:=$(shell uname -s)
	_libs:=m pthread

	ifeq ($(UNAME_S),Linux)
		ifeq ($(SDL2_STATIC_LINK),1)
			LFLAGS+=-Wl,-Bstatic -lSDL2 -Wl,-Bdynamic -lX11 -lXext -lXi -lXfixes -lXrandr -lXcursor
		else
			LFLAGS+=-lSDL2
		endif

		_libs+=dl
		# On POSX Use Address Sanitizers in Debug Mode
		ifeq ($(CC),gcc)
			ifeq ($(BUILD_TARGET),debug)
				CCFLAGS+=-fsanitize=address -fsanitize=undefined
				LFLAGS+=-fsanitize=address -fsanitize=undefined -lasan -lubsan
			endif
		endif
	endif
	ifeq ($(UNAME_S),Darwin)
		LFLAGS+=$(addprefix -framework , OpenGL Cocoa) -lSDL2
	endif

	LFLAGS+=$(addprefix -l,$(_libs))
endif

# make all WINDRES_TARGET=pe-x86-64(or pe-i386, WINDRES_TARGET is only needed on windows builds, this also requires make gen-rc)
all: $(bin)

-include $(DEPENDS)

%.o: %.rc
	@echo "WR  -" $<
	@windres.exe -O COFF -F $(WINDRES_TARGET) -i $< -o $@

%.o: %.c
	@echo "CC  -" $<
	@$(CC) --std=$(STD) $(CFLAGS) $(CCFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "CXX -" $<
	@$(CXX) --std=$(CXX_STD) $(CXXFLAGS) $(CCFLAGS) -c $< -o $@

$(bin): $(OBJS_C) $(OBJS_CPP)
	@echo Linking $@
	@$(CXX) --std=$(CXX_STD) $(OBJS_C) $(OBJS_CPP) $(LFLAGS) -o $@

.PHONY: run
.PHONY: clean

# make run
run: $(all)
	./$(bin)

# make clean
clean:
	@$(RM) $(bin) $(OBJS_C) $(OBJS_CPP) $(DEPENDS) ./csprite.exe.manifest windows.rc tools/font2inl.out data/*.ico data/icons/*.png src/assets/*.inl
	@echo Cleaned...

# make gen-rc Arch=x86_64(or i686)
gen-rc:
	@python3 tools/create_rc.py --arch=$(Arch) --majver=$(MajVer) --minver=$(MinVer) --patver=$(PatVer)
	@echo Generated RC...

# make gen-assets
gen-assets:
	@python3 tools/create_icons.py
	@echo Generated Icons...
	@python3 tools/create_assets.py
	@echo Generated Assets...

# make appimage
appimage:
	@echo Creating AppImage...
	@CSPRITE_VERSION=$(MajVer).$(MinVer).$(PatVer) appimage-builder --skip-test --recipe=AppImage-Builder.yml
	@echo Done!
