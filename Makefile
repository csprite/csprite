CXX := clang++
CXXSTD := c++17
CC := clang
CSTD := c99
LFLAGS := -Wall
DEFINATIONS :=# Possible Values = -DSHOW_FRAME_TIME -DSHOW_HISTORY_LOGS -DENABLE_WIN_ICON

CXXFLAGS := -std=${CXXSTD} # C++ Compiler Flags
CFLAGS := -std=${CSTD} # C Compiler Flags
CCFLAGS := -Wall -I. -Iinclude/ -Ilib/ # C & C++ Compiler Flags

#IMGUI v1.87

SRC:=src
LIB:=lib
OBJ:=obj

SRCS :=$(SRC)/main.cpp $(LIB)/glad.c $(LIB)/tinyfiledialogs.c
SRCS += $(LIB)/imgui/imgui.cpp $(LIB)/imgui/imgui_impl_opengl3.cpp
SRCS += $(LIB)/imgui/imgui_impl_glfw.cpp $(LIB)/imgui/imgui_draw.cpp
SRCS += $(LIB)/imgui/imgui_tables.cpp $(LIB)/imgui/imgui_widgets.cpp

OBJS :=$(OBJ)/main.o $(OBJ)/glad.o $(OBJ)/imgui.o $(OBJ)/tinyfiledialogs.o
OBJS += $(OBJ)/imgui_impl_opengl3.o $(OBJ)/imgui_impl_glfw.o
OBJS += $(OBJ)/imgui_draw.o $(OBJ)/imgui_tables.o $(OBJ)/imgui_widgets.o

ifeq ($(OS),Windows_NT)
	_ := $(shell cmd /c genRes.bat)
	LFLAGS += -lopengl32 -lgdi32 -lcomdlg32 -lole32 -lshell32
	OBJS += glfw3_mt.lib windows.o
	BIN := csprite.exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		LFLAGS += -lglfw -lm -ldl
		BIN := csprite
	endif
endif

all: CCFLAGS += -g -O0 -DIS_DEBUG
all: $(BIN)

release: CCFLAGS += -Os
release: DEFINATIONS += -DENABLE_WIN_ICON
release: $(BIN)

.PHONY: clean
clean:
	rm obj/*.o $(BIN)

# For Compiling OBJ/* To Binary
$(BIN): $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) -o $@

# For Compiling src/*.cpp
$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) $(CCFLAGS) $(DEFINATIONS) -c $< -o $@

# For Compiling lib/imgui/*.cpp
$(OBJ)/%.o: $(LIB)/imgui/%.cpp
	$(CXX) $(CXXFLAGS) $(CCFLAGS) -c $< -o $@

# For Compiling lib/*.cpp
$(OBJ)/%.o: $(LIB)/%.cpp
	$(CXX) $(CXXFLAGS) $(CCFLAGS) -c $< -o $@

# For Compiling lib/*.c
$(OBJ)/%.o: $(LIB)/%.c
	$(CC) $(CFLAGS) $(CCFLAGS) -c $< -o $@

# For Compiling src/*.c
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(CCFLAGS) -c $< -o $@
