# Requires GNU Make
CC       = gcc
AR       = ar
CXX      = g++
BUILD    = build
BIN      = $(BUILD)/csprite
FLAGS    = -march=native -MMD -MP -Wall -Wextra -Wpadded -pedantic -ffast-math -D_DEFAULT_SOURCE=1 -DCIMGUI_NO_EXPORT=1 -DIMGUI_IMPL_API="extern \"C\""
INCLUDES = src/ vendor/glad/include/ vendor/log.c/include/ vendor/cimgui vendor/stb/include vendor/sfd/src
CFLAGS   = -std=c99 $(addprefix -I,$(INCLUDES)) -fvisibility=hidden -DCIMGUI_USE_GLFW=1 -DCIMGUI_USE_OPENGL3=1 -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS=1
CXXFLAGS = $(addprefix -I,$(INCLUDES)) -fvisibility=hidden
LDFLAGS  = -fvisibility=hidden
LIBS     = vendor/glad/build/glad.a vendor/sfd/build/sfd.a
SOURCES  = $(addprefix src/,main.c app/gui.c app/render.c app/editor.c os/os.c os/gfx.c base/arena.c base/string.c bitmap/bitmap.c cimgui/impl.cpp assets/assets.c gfx/gfx.c) $(addprefix vendor/,log.c/src/log.c stb/impl.c)
OBJECTS  = $(patsubst %.c,%.c.o,$(patsubst %.cpp,%.cpp.o,$(SOURCES)))
OBJECTS := $(patsubst %,$(BUILD)/%,$(OBJECTS))
DEPENDS  = $(OBJECTS:.o=.d)

-include config.mk

# Check if `bear` command is available, Bear is used to generate
# `compile_commands.json` for your LSP to use, but can be disabled
# in command line by `make all BEAR=''`
# URL: github.com/rizsotto/Bear
# Note: Using multiple jobs with bear is not supported, i.e.
#       `make all -j4` won't work with bear enabled
BEAR :=
ifneq (, $(shell which bear))
	BEAR:=bear --append --output $(BUILD)/compile_commands.json --
endif

-include $(DEPENDS)

all: $(BIN)

vendor/sfd/build/sfd.a:
	@$(MAKE) --no-print-directory -C vendor/sfd/ all BUILD=build AR=$(AR) CC=$(CC) CXX=$(CXX) FLAGS='-O3' BACKEND=$(SFD_BACKEND)

vendor/glad/build/glad.a:
	@$(MAKE) --no-print-directory -C vendor/glad/ all BUILD=build AR=$(AR) CC=$(CC) CXX=$(CXX) FLAGS='-O3'

$(BUILD)/%.c.o: %.c
	@echo "Compile $<"
	@mkdir -p "$$(dirname "$@")"
	@$(BEAR) $(CC) $(FLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/%.cpp.o: %.cpp
	@echo "Compile $<"
	@mkdir -p "$$(dirname "$@")"
	@$(BEAR) $(CXX) $(FLAGS) $(CXXFLAGS) -c $< -o $@

$(BIN): $(OBJECTS) $(LIBS)
	@echo "   Link $@"
	@$(CXX) $(OBJECTS) $(LIBS) $(LDFLAGS) -o $@

$(eval PYTHON := $(if $(PYTHON),$(PYTHON),python3))

gen-assets:
	@echo "Produce src/assets/assets.inl"
	@$(PYTHON) tools/create_icons.py
	@$(PYTHON) tools/create_assets.py --cxx=$(CXX)

.PHONY: run clean

run: all
	@./$(BIN)

clean:
	@$(RM) -rv $(BIN) $(BUILD) src/assets/assets.inl
	@$(MAKE) --no-print-directory -C vendor/sfd/ clean
	@$(MAKE) --no-print-directory -C vendor/glad/ clean
