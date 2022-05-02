CXX := clang++
CC := clang
STD := c++17
LFLAGS := -I. -std=${STD} -Wall
CFLAGS := -Wall

#IMGUI v1.87

SRC=src
LIB=lib
OBJ=obj

SRCS=$(SRC)/main.cpp $(LIB)/glad.c $(LIB)/tinyfiledialogs.c
SRCS += $(LIB)/imgui/imgui.cpp $(LIB)/imgui/imgui_impl_opengl3.cpp
SRCS += $(LIB)/imgui/imgui_impl_glfw.cpp $(LIB)/imgui/imgui_draw.cpp
SRCS += $(LIB)/imgui/imgui_tables.cpp $(LIB)/imgui/imgui_widgets.cpp

OBJS=$(OBJ)/main.o $(OBJ)/glad.o $(OBJ)/imgui.o $(OBJ)/tinyfiledialogs.o
OBJS += $(OBJ)/imgui_impl_opengl3.o $(OBJ)/imgui_impl_glfw.o
OBJS += $(OBJ)/imgui_draw.o $(OBJ)/imgui_tables.o $(OBJ)/imgui_widgets.o

ifeq ($(OS),Windows_NT)
	LFLAGS += -lopengl32 -lgdi32 -lcomdlg32 -lole32 -lshell32
	OBJS += glfw3_mt.lib
	BIN = csprite.exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		LFLAGS += -lglfw -lm -ldl
		BIN = csprite
	endif
endif

all: CFLAGS += -g -O0
all: $(BIN)

release: CFLAGS += -O2 -DNDEBUG
release: $(BIN)

.PHONY: clean
clean:
	rm obj/*.o $(BIN)

# For Compiling OBJ/* To Binary
$(BIN): $(OBJS)
	$(CXX) $(LFLAGS) $(OBJS) -o $@

# For Compiling src/*.cpp
$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# For Compiling lib/imgui/*.cpp
$(OBJ)/%.o: $(LIB)/imgui/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

# For Compiling lib/*.c
$(OBJ)/%.o: $(LIB)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# For Compiling lib/*.cpp
$(OBJ)/%.o: $(LIB)/%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@
