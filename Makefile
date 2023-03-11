# simple hacky way to convert strings to lowercase, usage: NEW_VAR = $(call lc,$(VAR))
lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

MajVer:=1
MinVer:=0
PatVer:=0
Arch:=x86_64 # Used By gen-rc task

CXX:=g++
CC:=gcc
STD:=c99
CXX_STD:=c++11
CCFLAGS:=-Wall -Iinclude/ -Ithird_party/imgui/ -Ithird_party/FileBrowser/ -Ithird_party/ -MMD -MP -DCS_VERSION_MAJOR=$(MajVer) -DCS_VERSION_MINOR=$(MinVer) -DCS_VERSION_PATCH=$(PatVer) -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1 -DLOG_USE_COLOR=1
CFLAGS:=
LFLAGS:=

PYTHON:=python3
ZLIB_STATIC_LINK:=1
SDL2_STATIC_LINK:=1
SDL2_LFLAGS:=
SDL2_CFLAGS:=-DSDL_MAIN_HANDLED=1
WINDRES_TARGET:=pe-x86-64 # pe-x86-64 for 64 bit system or pe-i686 for 32 bit system
BUILD_TARGET:=debug

ifeq ($(call lc,$(BUILD_TARGET)),debug)
	CCFLAGS+=-DCS_BUILD_STABLE=0
else
	ifeq ($(call lc,$(BUILD_TARGET)),release)
		CCFLAGS+=-DCS_BUILD_STABLE=1 -DIMGUI_DISABLE_DEMO_WINDOWS=1 -DIMGUI_DISABLE_DEBUG_TOOLS=1
	else
$(error Invalid Build Target: "$(BUILD_TARGET)")
	endif
endif

SRCS_C:=$(wildcard src/*.c) $(wildcard src/**/*.c) $(wildcard third_party/**/*.c)
SRCS_CPP:=$(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard third_party/**/*.cpp)
OBJS_C:=$(SRCS_C:.c=.o)
OBJS_CPP:=$(SRCS_CPP:.cpp=.o)
DEPENDS:=$(patsubst %.c,%.d,$(SRCS_C)) $(patsubst %.cpp,%.d,$(SRCS_CPP))
bin:=csprite

ifeq ($(call lc,$(BUILD_TARGET)),debug)
	CCFLAGS+=-O0 -g
else
	CCFLAGS+=-O3 -fdata-sections -ffunction-sections -flto
	LFLAGS+=-Wl,--gc-sections -flto # on mac this is replaced with -dead_strip below.
endif

ifeq ($(OS),Windows_NT)
	ifeq ($(SDL2_STATIC_LINK),1)
		SDL2_LFLAGS+=-static-libstdc++ -Wl,-Bstatic -lSDL2main -lSDL2 -Wl,-Bdynamic
	else
		SDL2_LFLAGS+=-lSDL2main -lSDL2
	endif
	ifeq ($(ZLIB_STATIC_LINK),1)
		LFLAGS+=-Wl,-Bstatic -lz -Wl,-Bdynamic
	else
		LFLAGS+=-lz
	endif

	LFLAGS+=-lopengl32
	SDL2_LFLAGS+=$(addprefix -l,winmm gdi32 imm32 ole32 oleaut32 shell32 version uuid setupapi)
	ifeq ($(call lc,$(BUILD_TARGET)),debug)
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
			SDL2_LFLAGS+=-Wl,-Bstatic -lSDL2 -Wl,-Bdynamic -lX11 -lXext -lXi -lXfixes -lXrandr -lXcursor
		else
			SDL2_LFLAGS+=-lSDL2
		endif
		ifeq ($(ZLIB_STATIC_LINK),1)
			LFLAGS+=-Wl,-Bstatic -lz -Wl,-Bdynamic
		else
			LFLAGS+=-lz
		endif

		_libs+=dl
		# On Linux Use Address Sanitizers in Debug Mode
		ifeq ($(call lc,$(BUILD_TARGET)),debug)
			CCFLAGS+=-fsanitize=address -fsanitize=undefined
			LFLAGS+=-fsanitize=address -fsanitize=undefined
		endif
	endif
	ifeq ($(UNAME_S),Darwin)
		ifeq ($(call lc,$(BUILD_TARGET)),release)
			LFLAGS:=$(subst --gc-sections,-dead_strip,$(LFLAGS)) # replace --gc-sections with -dead_strip
		endif
		LFLAGS+=$(addprefix -framework , OpenGL Cocoa) -lz
		SDL2_LFLAGS:=-lSDL2
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
	@$(CC) --std=$(STD) $(CFLAGS) $(CCFLAGS) $(SDL2_CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "CXX -" $<
	@$(CXX) --std=$(CXX_STD) $(CXXFLAGS) $(CCFLAGS) $(SDL2_CFLAGS) -c $< -o $@

$(bin): $(OBJS_C) $(OBJS_CPP)
	@echo Linking $@
	@$(CXX) --std=$(CXX_STD) $(OBJS_C) $(OBJS_CPP) $(LFLAGS) $(SDL2_LFLAGS) -o $@

.PHONY: run
.PHONY: clean

# make run
run: $(bin)
	@./$(bin)

# make clean
clean:
	@$(RM) $(bin) $(OBJS_C) $(OBJS_CPP) $(DEPENDS) ./csprite.exe.manifest windows.rc tools/font2inl.out data/*.ico data/icons/*.png src/assets/*.inl
	@echo - Cleaned

# make gen-rc Arch=x86_64(or i686)
gen-rc:
	@$(PYTHON) tools/create_rc.py --arch=$(Arch) --majver=$(MajVer) --minver=$(MinVer) --patver=$(PatVer)
	@echo - RC generated

# make gen-assets
gen-assets:
	@$(PYTHON) tools/create_icons.py
	@echo - Icons generated
	@$(PYTHON) tools/create_assets.py --cxx=$(CXX)
	@echo - Assets generated

# make appimage
appimage:
	@CSPRITE_VERSION=$(MajVer).$(MinVer).$(PatVer) appimage-builder --skip-test --recipe=AppImage-Builder.yml
	@echo - AppImage created


