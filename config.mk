# Statically Link with GLFW
GLFW_STATIC=false
# Enable Colored Output in logs
LOG_ENABLE_COLOR=true
# Possible Values: debug, release
BUILD_TYPE = debug
# SimpleFileDialog Backend: win32, zenity
SFD_BACKEND =
# Build & Bin
BUILD = build
BIN = $(BUILD)/csprite

# Append Variables According To Config

ifeq ($(OS),Windows_NT)
	CFLAGS += -DTARGET_WINDOWS
	SFD_BACKEND = win32
	LFLAGS += -lcomdlg32
	BIN += .exe
else
	SFD_BACKEND = zenity
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CFLAGS += -DTARGET_LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		CFLAGS += -DTARGET_APPLE
	endif
endif

ifeq ($(BUILD_TYPE),debug)
	FLAGS += -O0 -g3 -fsanitize=address,undefined
	LDFLAGS += -fsanitize=address,undefined
else
	ifeq ($(BUILD_TYPE),release)
		FLAGS += -O3
	else
$(error Unknown build type "$(BUILD_TYPE)", valid values: debug, release)
	endif
endif

ifeq ($(GLFW_STATIC),true)
	LDFLAGS += -Wl,-Bstatic -lglfw -Wl,-Bdynamic
else
	LDFLAGS += -lglfw
endif

ifeq ($(LOG_ENABLE_COLOR),true)
	CFLAGS += -DLOG_USE_COLOR=1
endif
