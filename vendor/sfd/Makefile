CC      = gcc
AR      = ar
FLAGS   =
CFLAGS  = -std=c99 -Iinclude/
BUILD   = build
BIN     = $(BUILD)/sfd.a
SOURCES = src/sfd.c
OBJECTS = $(patsubst %,$(BUILD)/%,$(SOURCES:.c=.c.o))
BACKEND =

ifeq ($(BACKEND),win32)
	CFLAGS += -DSFD_BACKEND_WIN32
else
	ifeq ($(BACKEND),zenity)
		CFLAGS += -DSFD_BACKEND_ZENITY
	endif
endif

all: $(BIN)

$(BUILD)/%.c.o: %.c
	@echo "Compile vendor/sfd/$<"
	@mkdir -p "$$(dirname "$@")"
	@$(CC) $(FLAGS) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJECTS)
	@echo "Archive vendor/sfd/$@"
	@$(AR) rcs $(BIN) $(OBJECTS)

.PHONY: clean

clean:
	@$(RM) -rv $(BIN) $(BUILD)
