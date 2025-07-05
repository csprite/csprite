# csprite
A simple pixel art editor.

![csprite ui preview](https://csprite.github.io/media/csprite-preview.png)

> [!NOTE]
> _This README is intended as a technical overview of
> the software, Intended for developers. If you're an user, Please
> visit [csprite.github.io](https://csprite.github.io)_

Minimum System Requirements:
- Linux, Windows 7 or Later (Future plans to support Mac & Mobile)
- Atleast 512MB of Usable RAM
- 64-bit Processor (Future plans to support 32-bit processors in future)
- OpenGL v3.0 or Later

You can download pre-built binaries of the software [here](https://github.com/csprite/csprite/actions/workflows/build.yml?query=branch%3Ac) (Requires login).

The main aim of this software is to be simple on it's own
in terms of code & UX. With functionality to write plugins
to add support for various things like File Formats, etc.

## Compiling

Requirements for Windows:
- Windows 7 or Later
- VS Build Tools 2019 or Later With [Clang Support](https://learn.microsoft.com/en-us/cpp/build/clang-support-msbuild)
  ([Direct Download](https://aka.ms/vs/16/release/vs_BuildTools.exe))
- BusyBox for Windows ([Direct Download](https://frippery.org/files/busybox/busybox.exe))
- Python v3.0+ ([Direct Download](https://www.python.org/downloads)) (Make sure to select "Add To Path" option)

Requirements for Linux:
- POSIX Compliant Shell
- [GCC](https://repology.org/project/gcc/versions) or [Clang](https://repology.org/project/clang/versions)
  (And [libomp-dev](https://packages.debian.org/search?keywords=libomp-dev) if using Clang)
  
  > On Debian/Ubuntu (And maybe other distros as well), If you're using LLVM-Clang
  > toolchain then `libomp-dev` has to be installed as it doesn't come packaged with
  > the toolchain for some reason.
- [GLFW3](https://repology.org/project/glfw/versions) v3.1 or Later.
- [MOLD - Modern Linker](https://github.com/rui314/mold)
  
  > MOLD is used for speeding up the linking process. It is completely optional & Even
  > unnecessary if you're just building for use. It was able to cut my linking time
  > by a second or two which makes things a bit less annoying.
- [Python v3.0+](https://repology.org/project/python3/versions)

## Roadmap
- [ ] Tool Preview - Just have a buffer on which user's action
      are cleared (by storing dirty from previous user input) &
      drawn on every user input. When it's time to commit just
      blit the buffer or blend it?
- [ ] Variable Brush/Eraser Sizes
- [ ] Undo Redo
- [ ] Layers with Blending Modes
  - [ ] Alpha
  - [ ] Addition
  - [ ] Subtraction
  - [ ] Difference
  - [ ] Multiply
  - [ ] Screen
  - [ ] Overlay
  - [ ] Darken
  - [ ] Lighten
  - [ ] Color Dodge/Burn
  - [ ] Hard/Soft Light
- [ ] Plugin System
- [ ] Node Based Post Processing Effects
  - [ ] Basic Effects like Lightness, Saturation, etc.
  - [ ] Dithering Effects (Like DitherBoy)
  - [ ] Convolutional Filters
- [ ] Unit Testing (<https://youtu.be/21JlBOxgGwY>)
- [ ] UTF-8 Support (Maybe)
- [ ] Docking (Maybe)
- [ ] Color Pickers for Normal, Specular, Roughness & Height Maps (<https://youtu.be/gUkY8ZoRfuQ>)
- [ ] Generate Normal Map Functionality (<https://youtu.be/-rJdOc9WZS4>)
- [ ] Procedural Math Based Art (Maybe)
- [ ] MacOS Port
- [ ] Mobile Port
- [ ] Misc
  - [ ] [Porter Duff's Alpha Functions](https://www.pismin.com/10.1145/800031.808606)
  - [ ] <https://lodev.org/cgtutor/painting.html>
  - [ ] <https://lodev.org/cgtutor/floodfill.html#Scanline_Floodfill_Algorithm_With_Stack>

## References

Here are resources that have helped me while developing
this software.

- [It's probably time to stop recommending Clean Code](https://qntm.org/clean)
- [Enter The Arena: Simplifying Memory Management (2023)](https://youtu.be/TZ5a3gCCZYo)
- [Compositing and Blending Level 1](https://www.w3.org/TR/compositing-1/)
