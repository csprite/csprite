# simple hacky way to convert strings to lowercase, usage: NEW_VAR = $(call lc,$(VAR))
lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

MajVer:=0
MinVer:=1
PatVer:=1
Arch:=x86_64 # Used By gen-rc task

CXX:=g++
CC:=gcc
STD:=c99
CXX_STD:=c++11
CCFLAGS:=-Iinclude/ -Ilib/imgui/ -Ilib/ -Wall -MMD -MP -DCS_VERSION_MAJOR=$(MajVer) -DCS_VERSION_MINOR=$(MinVer) -DCS_VERSION_PATCH=$(PatVer) -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1
CFLAGS:=
LFLAGS:=

PYTHON:=python3
SDL2_STATIC_LINK:=1
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

SRCS_C:=$(wildcard src/*.c) $(wildcard lib/*.c) $(wildcard lib/**/*.c)
SRCS_CPP:=$(wildcard src/*.cpp) $(wildcard lib/**/*.cpp)
OBJS_C:=$(SRCS_C:.c=.o)
OBJS_CPP:=$(SRCS_CPP:.cpp=.o)
DEPENDS:=$(patsubst %.c,%.d,$(SRCS_C)) $(patsubst %.cpp,%.d,$(SRCS_CPP))
bin:=csprite

ifeq ($(call lc,$(BUILD_TARGET)),debug)
	CCFLAGS+=-O0 -g
else
	CCFLAGS+=-O3
endif

ifeq ($(OS),Windows_NT)
	LFLAGS+=-static-libstdc++ -Wl,-Bstatic -lglfw3 -Wl,-Bdynamic $(addprefix -l,opengl32 gdi32 comdlg32 ole32 shell32)
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

	ifeq ($(UNAME_S),Linux)
		# On POSX Use Address Sanitizers in Debug Mode
		ifeq ($(CC),gcc)
			ifeq ($(call lc,$(BUILD_TARGET)),debug)
				CCFLAGS+=-fsanitize=address -fsanitize=undefined
				LFLAGS+=-fsanitize=address -fsanitize=undefined -lasan -lubsan
			endif
		endif
	endif
	ifeq ($(UNAME_S),Darwin)
		LFLAGS+=$(addprefix -framework , OpenGL Cocoa) -lobjc
	endif

	LFLAGS+=-lglfw -ldl -lm -lpthread
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
run: $(bin)
	@./$(bin)

# make clean
clean:
	@$(RM) $(bin) $(OBJS_C) $(OBJS_CPP) $(DEPENDS) ./csprite.exe.manifest windows.rc tools/font2inl.out data/*.ico data/icons/*.png src/assets/*.inl
	@echo Cleaned...

# make gen-rc Arch=x86_64(or i686)
gen-rc:
	@$(PYTHON) tools/create_rc.py --arch=$(Arch) --majver=$(MajVer) --minver=$(MinVer) --patver=$(PatVer)
	@echo Generated RC...

# make gen-assets
gen-assets:
	@$(PYTHON) tools/create_icons.py
	@echo Generated Icons...
	@$(PYTHON) tools/create_assets.py --cxx=$(CXX)
	@echo Generated Assets...

# make appimage
appimage:
	@echo Creating AppImage...
	@CSPRITE_VERSION=$(MajVer).$(MinVer).$(PatVer) appimage-builder --skip-test --recipe=AppImage-Builder.yml
	@echo Done!

