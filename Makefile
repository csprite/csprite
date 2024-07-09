# Requires GNU Make
CC       = gcc
CXX      = g++
FLAGS    = -MMD -MP -Wall -Wextra -pedantic -Ivendor/cimgui/ -DCIMGUI_USE_GLFW=1 -DCIMGUI_USE_OPENGL3=1 -DCIMGUI_NO_EXPORT=1
CFLAGS   = -std=c99 -Isrc/ -Ivendor/glad/include/ -Ivendor/log.c/include/ -DCIMGUI_DEFINE_ENUMS_AND_STRUCTS=1
CXXFLAGS = -std=c++11 -DIMGUI_IMPL_API="extern \"C\""
LDFLAGS  = -lm
BUILD    = build
BIN      = $(BUILD)/csprite
SRC_C    = $(addprefix src/,main.c app/window.c) $(addprefix vendor/,glad/glad.c log.c/src/log.c)
SRC_CPP  = $(addprefix vendor/cimgui/,cimgui.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp imgui/imgui_impl_opengl3.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp)
OBJECTS  = $(patsubst %,$(BUILD)/%,$(SRC_C:.c=.c.o)) $(patsubst %,$(BUILD)/%,$(SRC_CPP:.cpp=.cpp.o))
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

ifeq ($(GLFW_STATIC),true)
	LDFLAGS += -Wl,-Bstatic -lglfw -Wl,-Bdynamic
else
	LDFLAGS += -lglfw
endif

ifeq ($(LOG_ENABLE_COLOR),true)
	CFLAGS += -DLOG_USE_COLOR=1
endif

-include $(DEPENDS)

all: $(BIN)

$(BUILD)/%.c.o: %.c
	@echo "CC  -" $<
	@mkdir -p "$$(dirname "$@")"
	@$(BEAR) $(CC) $(FLAGS) $(CFLAGS) -c $< -o $@

$(BUILD)/%.cpp.o: %.cpp
	@echo "CXX -" $<
	@mkdir -p "$$(dirname "$@")"
	@$(BEAR) $(CXX) $(FLAGS) $(CXXFLAGS) -c $< -o $@

$(BIN): $(OBJECTS)
	@echo "LD  -" $@
	@$(CXX) $(OBJECTS) $(LDFLAGS) -o $@

.PHONY: run clean

run: all
	@./$(BIN)

clean:
	@$(RM) -rv $(BIN) $(BUILD)
