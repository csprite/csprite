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
	@echo "CC  -" $<
	@mkdir -p "$$(dirname "$@")"
	@$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJECTS)
	@echo "AR  -" $@
	@$(AR) rcs $(BIN) $(OBJECTS)

.PHONY: clean

clean:
	@$(RM) -rv $(BIN) $(BUILD)
