# Requires GNU Make
CC       = gcc
CXX      = g++
FLAGS    = -MMD -MP -Wall -Wextra -pedantic
INCLUDES = src/ vendor/glad/include/ vendor/log.c/include/ vendor/cimgui
CFLAGS   = -std=c99 $(addprefix -I,$(INCLUDES)) -DCIMGUI_USE_GLFW=1 -DCIMGUI_USE_OPENGL3=1 -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS=1
LDFLAGS  =
BUILD    = build
BIN      = $(BUILD)/csprite
SOURCES  = $(addprefix src/,main.c app/window.c) $(addprefix vendor/,glad/glad.c log.c/src/log.c)
OBJECTS  = $(patsubst %,$(BUILD)/%,$(SOURCES:.c=.c.o))
DEPENDS  = $(OBJECTS:.o=.d)

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

-include config.mk

-include $(DEPENDS)

all: $(BIN)

cimgui:
	@$(MAKE) --no-print-directory -C vendor/cimgui/ all BUILD=build FLAGS='-O3 -DIMGUI_IMPL_API="extern \"C\""'

$(BUILD)/%.c.o: %.c
	@echo "CC  -" $<
	@mkdir -p "$$(dirname "$@")"
	@$(BEAR) $(CC) $(FLAGS) $(CFLAGS) -c $< -o $@

$(BIN): cimgui $(OBJECTS)
	@echo "LD  -" $@
	@$(CXX) $(OBJECTS) vendor/cimgui/build/cimgui.a $(LDFLAGS) -o $@

.PHONY: run clean

run: all
	@./$(BIN)

clean:
	@$(RM) -rv $(BIN) $(BUILD)
	@$(MAKE) --no-print-directory -C vendor/cimgui/ clean
