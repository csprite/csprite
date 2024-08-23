# Requires GNU Make
CC       = gcc
AR       = ar
CXX      = g++
FLAGS    = -MMD -MP -Wall -Wextra -pedantic
INCLUDES = src/ vendor/glad/include/ vendor/log.c/include/ vendor/cimgui vendor/stb/include vendor/sfd/src
CFLAGS   = -std=c99 $(addprefix -I,$(INCLUDES)) -DCIMGUI_USE_GLFW=1 -DCIMGUI_USE_OPENGL3=1 -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS=1
LDFLAGS  =
LIBS     = vendor/cimgui/build/cimgui.a vendor/glad/build/glad.a vendor/sfd/build/sfd.a
SOURCES  = $(addprefix src/,main.c assets/assets.c app/app.c app/window.c app/texture.c app/editor.c image/image.c fs/fs.c gfx/gfx.c) $(addprefix vendor/,log.c/src/log.c stb/impl.c)
OBJECTS  = $(patsubst %,$(BUILD)/%,$(SOURCES:.c=.c.o))
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

vendor/cimgui/build/cimgui.a:
	@$(MAKE) --no-print-directory -C vendor/cimgui/ all BUILD=build AR=$(AR) CC=$(CC) CXX=$(CXX) FLAGS='-O3 -DIMGUI_IMPL_API="extern \"C\""'

vendor/glad/build/glad.a:
	@$(MAKE) --no-print-directory -C vendor/glad/ all BUILD=build AR=$(AR) CC=$(CC) CXX=$(CXX) FLAGS='-O3'

$(BUILD)/%.c.o: %.c
	@echo "CC  -" $<
	@mkdir -p "$$(dirname "$@")"
	@$(BEAR) $(CC) $(FLAGS) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJECTS) $(LIBS)
	@echo "LD  -" $@
	@$(CXX) $(OBJECTS) $(LIBS) $(LDFLAGS) -o $@

$(eval PYTHON := $(if $(PYTHON),$(PYTHON),python3))

gen-assets:
	@$(PYTHON) tools/create_icons.py
	@echo "PY  -" tools/create_icons.py
	@$(PYTHON) tools/create_assets.py --cxx=$(CXX)
	@echo "PY  -" tools/create_assets.py

.PHONY: run clean

run: all
	@./$(BIN)

clean:
	@$(RM) -rv $(BIN) $(BUILD) src/assets/assets.inl
	@$(MAKE) --no-print-directory -C vendor/cimgui/ clean
	@$(MAKE) --no-print-directory -C vendor/sfd/ clean
	@$(MAKE) --no-print-directory -C vendor/glad/ clean
