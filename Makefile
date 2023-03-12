# simple hacky way to convert strings to lowercase, usage: NEW_VAR = $(call lc,$(VAR))
lc = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$1))))))))))))))))))))))))))

MajVer:=1
MinVer:=0
PatVer:=0

LD:=g++
CC:=gcc
CXX:=g++

C_FLAGS:=--std=c99
CPP_FLAGS:=--std=c++11 -Ithird_party/imgui/ -Ithird_party/FileBrowser/
LD_FLAGS:=

FLAGS = -MMD -MP -Wall -Iinclude/ -Ithird_party/
FLAGS += -DCS_VERSION_MAJOR=$(MajVer) -DCS_VERSION_MINOR=$(MinVer) -DCS_VERSION_PATCH=$(PatVer)
FLAGS += -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1 -DLOG_USE_COLOR=1

ZLIB_STATIC_LINK:=1
SDL2_STATIC_LINK:=1
SDL2_LDFLAGS:=
SDL2_CFLAGS:=-DSDL_MAIN_HANDLED=1
WINDRES_TARGET:=pe-x86-64 # pe-x86-64 for 64 bit system or pe-i686 for 32 bit system
BUILD_TARGET:=debug

ifeq ($(call lc,$(BUILD_TARGET)),debug)
	FLAGS+=-DCS_BUILD_STABLE=0
else
	ifeq ($(call lc,$(BUILD_TARGET)),release)
		FLAGS+=-DCS_BUILD_STABLE=1 -DIMGUI_DISABLE_DEMO_WINDOWS=1 -DIMGUI_DISABLE_DEBUG_TOOLS=1
	else
$(error Invalid Build Target: "$(BUILD_TARGET)")
	endif
endif

ODIR     = build
BIN      = csprite
SRCS_C   = $(wildcard src/*.c) $(wildcard src/**/*.c) $(wildcard third_party/**/*.c)
SRCS_CPP = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp) $(wildcard third_party/**/*.cpp)
OBJECTS  = $(patsubst %,$(ODIR)/%.o,$(SRCS_C))
OBJECTS += $(patsubst %,$(ODIR)/%.o,$(SRCS_CPP))
DEPENDS := $(OBJECTS:.o=.d)

ifeq ($(call lc,$(BUILD_TARGET)),debug)
	FLAGS+=-O0 -g
else
	FLAGS+=-O3 -fdata-sections -ffunction-sections -flto
	LD_FLAGS+=-Wl,--gc-sections -flto # on mac this is replaced with -dead_strip below.
endif

ifeq ($(OS),Windows_NT)
	ifeq ($(SDL2_STATIC_LINK),1)
		SDL2_LDFLAGS+=-static-libstdc++ -Wl,-Bstatic -lSDL2main -lSDL2 -Wl,-Bdynamic
	else
		SDL2_LDFLAGS+=-lSDL2main -lSDL2
	endif
	ifeq ($(ZLIB_STATIC_LINK),1)
		LD_FLAGS+=-Wl,-Bstatic -lz -Wl,-Bdynamic
	else
		LD_FLAGS+=-lz
	endif

	LD_FLAGS+=-lopengl32
	SDL2_LDFLAGS+=$(addprefix -l,winmm gdi32 imm32 ole32 oleaut32 shell32 version uuid setupapi)
	ifeq ($(call lc,$(BUILD_TARGET)),debug)
		LD_FLAGS+=-mconsole
	else
		LD_FLAGS+=-mwindows
	endif
	SRCS_C+=windows.rc
	OBJECTS+=windows.rc.o
	BIN=csprite.exe
else
	UNAME_S:=$(shell uname -s)
	_libs:=m pthread

	ifeq ($(UNAME_S),Linux)
		ifeq ($(SDL2_STATIC_LINK),1)
			SDL2_LDFLAGS+=-Wl,-Bstatic -lSDL2 -Wl,-Bdynamic -lX11 -lXext -lXi -lXfixes -lXrandr -lXcursor
		else
			SDL2_LDFLAGS+=-lSDL2
		endif
		ifeq ($(ZLIB_STATIC_LINK),1)
			LD_FLAGS+=-Wl,-Bstatic -lz -Wl,-Bdynamic
		else
			LD_FLAGS+=-lz
		endif

		_libs+=dl
		# On Linux Use Address Sanitizers in Debug Mode
		ifeq ($(call lc,$(BUILD_TARGET)),debug)
			FLAGS+=-fsanitize=address -fsanitize=undefined
			LD_FLAGS+=-fsanitize=address -fsanitize=undefined
		endif
	endif
	ifeq ($(UNAME_S),Darwin)
		ifeq ($(call lc,$(BUILD_TARGET)),release)
			LD_FLAGS:=$(subst --gc-sections,-dead_strip,$(LD_FLAGS)) # replace --gc-sections with -dead_strip
		endif
		LD_FLAGS+=$(addprefix -framework , OpenGL Cocoa) -lz
		SDL2_LDFLAGS:=-lSDL2
	endif

	LD_FLAGS+=$(addprefix -l,$(_libs))
endif

-include $(DEPENDS)

$(ODIR)/%.rc.o: %.rc
	@echo "WR  -" $<
	@mkdir -p "$$(dirname "$@")"
	@windres.exe -i $< -o $@ -O COFF -F $(WINDRES_TARGET)

$(ODIR)/%.c.o: %.c
	@echo "CC  -" $<
	@mkdir -p "$$(dirname "$@")"
	@$(CC) -c $< -o $@ $(FLAGS) $(C_FLAGS) $(SDL2_CFLAGS)

$(ODIR)/%.cpp.o: %.cpp
	@echo "CXX -" $<
	@mkdir -p "$$(dirname "$@")"
	@$(CXX) -c $< -o $@ $(FLAGS) $(CPP_FLAGS) $(SDL2_CFLAGS)

$(BIN): $(OBJECTS)
	@echo Linking $@
	@$(LD) -o $@ $(OBJECTS) $(LD_FLAGS) $(SDL2_LDFLAGS)

# make all WINDRES_TARGET=pe-x86-64(or pe-i386, WINDRES_TARGET is only needed on windows builds, this also requires make gen-rc)
all: $(BIN)

.PHONY: run
.PHONY: clean

# make run
run: $(BIN)
	@./$(BIN)

# make clean
clean:
	@$(RM) -r $(BIN) $(ODIR) $(DEPENDS) src/assets/*.inl data/*.ico data/icons/*.png ./csprite.exe.manifest windows.rc tools/font2inl.out
	@echo - Cleaned

# make gen-rc ARCH=x86_64(or i686) PYTHON=python3
gen-rc:
	$(eval ARCH := $(if $(ARCH),$(ARCH),x86_64))
	$(eval PYTHON := $(if $(PYTHON),$(PYTHON),python3))
	@$(PYTHON) tools/create_rc.py --arch=$(ARCH) --majver=$(MajVer) --minver=$(MinVer) --patver=$(PatVer)
	@echo - RC generated

# make gen-assets PYTHON=python3
gen-assets:
	$(eval PYTHON := $(if $(PYTHON),$(PYTHON),python3))
	@$(PYTHON) tools/create_icons.py
	@echo - Icons generated
	@$(PYTHON) tools/create_assets.py --cxx=$(CXX)
	@echo - Assets generated

# make appimage
appimage:
	@CSPRITE_VERSION=$(MajVer).$(MinVer).$(PatVer) appimage-builder --skip-test --recipe=AppImage-Builder.yml
	@echo - AppImage created

