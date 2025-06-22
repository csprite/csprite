# Statically Link with GLFW
GLFW_STATIC=false
# Enable Colored Output in logs
LOG_ENABLE_COLOR=true
# Possible Values: debug, release
BUILD_TYPE = debug
# SimpleFileDialog Backend: win32, zenity
SFD_BACKEND =

# Append Variables According To Config

ifeq ($(OS),Windows_NT)
	FLAGS += -DTARGET_WINDOWS
	SFD_BACKEND = win32
	LFLAGS += -lcomdlg32
	BIN += .exe
else
	SFD_BACKEND = zenity
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		FLAGS += -DTARGET_LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		FLAGS += -DTARGET_APPLE
	endif
endif

ifeq ($(BUILD_TYPE),debug)
	FLAGS += -O0 -g3 -fsanitize=address,undefined -DBUILD_DEBUG=1
	LDFLAGS += -fsanitize=address,undefined
else
	ifeq ($(BUILD_TYPE),release)
		# TODO(pegvin) - Look into https://stackoverflow.com/q/6687630/14516016
		# in detail & figure out a way to strip all the unused functions, Since
		# we won't be used most of the ImGui's functions anyways.
		FLAGS += -O3 -fdata-sections -ffunction-sections -DBUILD_RELEASE=1
		LDFLAGS += -Wl,--gc-sections
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
