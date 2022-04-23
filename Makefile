CC := clang
STD := c99
LFLAGS := -lglfw -lm -ldl -I. -std=${STD} -Wall
CFLAGS := -Wall

SRC=src
LIB=lib
OBJ=obj

SRCS=$(SRC)/main.c $(LIB)/glad.c
OBJS=$(OBJ)/main.o $(OBJ)/glad.o
BIN=csprite

COMMIT_HASH=$(shell git rev-parse --short HEAD)
VERSION=0.5.0

CFLAGS += -DCOMMIT_HASH="\"$(COMMIT_HASH)\"" -DPROGRAM_VERSION="\"$(VERSION)\""

all: LFLAGS += -g -O0
all: $(BIN)

release: LFLAGS += -O2 -DNDEBUG
release: $(BIN)

.PHONY: clean
clean:
	rm obj/*.o $(BIN)

# For Compiling OBJ/* To Binary
$(BIN): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $@

# For Compiling Src/*
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# For Compiling Lib/*
$(OBJ)/%.o: $(LIB)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
