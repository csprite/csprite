CXX      = g++
AR       = ar
FLAGS    =
CXXFLAGS = -std=c++11 -I. -DCIMGUI_NO_EXPORT=1 -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS=1 -DIMGUI_DISABLE_OBSOLETE_KEYIO=1
BUILD    = build
BIN      = $(BUILD)/cimgui.a
SOURCES  = cimgui.cpp imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_impl_glfw.cpp
SOURCES += imgui/imgui_impl_opengl3.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp
OBJECTS  = $(patsubst %,$(BUILD)/%,$(SOURCES:.cpp=.cpp.o))

all: $(BIN)

$(BUILD)/%.cpp.o: %.cpp
	@echo "CXX -" $<
	@mkdir -p "$$(dirname "$@")"
	@$(CXX) $(FLAGS) $(CXXFLAGS) -c $< -o $@

$(BIN): $(OBJECTS)
	@echo "AR  -" $@
	@$(AR) rcs $(BIN) $(OBJECTS)

.PHONY: clean

clean:
	@$(RM) -rv $(BIN) $(BUILD)
