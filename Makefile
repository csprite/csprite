CC := clang
STD := c99
CFLAGS := -lglfw -lm -ldl -I. -std=${STD} -Wall
LFLAGS := -Wall

SRC=src
LIB=lib
OBJ=obj

SRCS=$(SRC)/main.c $(LIB)/glad.c
OBJS=$(OBJ)/main.o $(OBJ)/glad.o
BIN=csprite

all: CFLAGS += -g -O0
all: $(BIN)

release: CFLAGS += -O2 -DNDEBUG
release: $(BIN)

.PHONY: clean
clean:
	rm obj/*.o $(BIN)

# For Compiling OBJ/* To Binary
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

# For Compiling Src/*
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(LFLAGS) -c $< -o $@

# For Compiling Lib/*
$(OBJ)/%.o: $(LIB)/%.c
	$(CC) $(LFLAGS) -c $< -o $@
