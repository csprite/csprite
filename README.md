# CSprite
A Pixel Art Tool - Powered By OpenGL & ImGui

---
# Install
## Prebuilt Binary:
Latest pre-built binary of CSprite for Linux & Windows can be found [Here](https://github.com/DEVLOPRR/CSprite/releases)

## Building From Source:

### Windows
1. Install Clang, Git, MinGW Make or Make For Windows
    - [Clang](https://github.com/llvm/llvm-project/releases) ( `LLVM-x.x.x-win64.exe` or `LLVM-x.x.x-win32.exe` )
    - [Git](https://git-scm.com/downloads)
    - [Make For Windows](http://gnuwin32.sourceforge.net/downlinks/make.php) (Skip if you have MinGW installed and `mingw32-make --version` shows some version)

2. Get The Source Code: `git clone https://github.com/DEVLOPRR/CSprite`
3. Build The Source: `make -j4`
    - Replace the `make` command with `mingw32-make` if you have it installed.
    - Replace `4` in `-j4` with the number of CPU cores you have to decreease the compile time.
    - Add `release` in the command to make a release build (i.e. 'optimized') (Example: `make release -j4`

### Linux

1. Install GLFW
    - Debian: `sudo apt-get install libglfw3 libglfw3-dev`
    - Arch: `yay -S glfw`
    - Or Build From Source, [Get GLFW](https://www.glfw.org/).

2. Install Required Tools: `make` `clang`
3. Get The Source Code: `git clone https://github.com/DEVLOPRR/CSprite`
4. Build The Source: `make release -j4` (Replace the `4` in `-j4` with the number of CPU Cores you have)

---

### Thanks
