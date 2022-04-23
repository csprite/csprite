CC := clang
STD := c99
CFLAGS := -lglfw -lm -ldl -I. -std=${STD} -g

SOURCES=src/main.c lib/glad.c
OBJECTS=$(SOURCES:.cpp=.o)
OUTPUT_BINARY=sixel

all: $(SOURCES) $(OUTPUT_BINARY)

$(OUTPUT_BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@
