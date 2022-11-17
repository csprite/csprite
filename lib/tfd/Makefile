CC := clang
LFLAGS :=
CFLAGS := -std=c89 -Wall -Wno-comment -Isrc/

TFD :=src/tinyfiledialogs.c
EXM :=examples

ifeq ($(OS),Windows_NT)
	LFLAGS += -lcomdlg32 -lole32 -luser32 -lshell32
endif

all:
	$(CC) $(CFLAGS) $(LFLAGS) -o $(EXM)/hello.elf $(EXM)/hello.c $(TFD)
	$(CC) $(CFLAGS) $(LFLAGS) -o $(EXM)/hello_wchar_t.elf $(EXM)/hello_wchar_t.c $(TFD)

.PHONY: clean
clean:
	rm -f $(EXM)/*.elf
