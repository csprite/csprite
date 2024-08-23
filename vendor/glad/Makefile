CC      = gcc
AR      = ar
FLAGS   =
CFLAGS  = -std=c99 -Iinclude/
BUILD   = build
BIN     = $(BUILD)/glad.a
SOURCES = glad.c
OBJECTS = $(patsubst %,$(BUILD)/%,$(SOURCES:.c=.c.o))

all: $(BIN)

$(BUILD)/%.c.o: %.c
	@echo "Compile vendor/glad/$<"
	@mkdir -p "$$(dirname "$@")"
	@$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJECTS)
	@echo "Archive vendor/glad/$@"
	@$(AR) rcs $(BIN) $(OBJECTS)

.PHONY: clean

clean:
	@$(RM) -rv $(BIN) $(BUILD)
