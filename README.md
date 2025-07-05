# csprite

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

## Todo
- Live Canvas Preview? The idea is simple. Just have a buffer
  on which user's action are drawn on every frame & Cleared
  every frame (By storing the dirty from previous frame) & When
  finally it's time to commit. You just blit the buffer or maybe
  blend it?
- Dithering Tool Like Dither Boy?
- Unit Testing? More like Block Testing. Instead of testing
  each function, Let's test subsystems or a block.
  - https://youtu.be/21JlBOxgGwY
- UTF-8 Support?
- Docking? The main idea is to have 3 panels, Left, Right & Bottom.
  These panels will allow windows to be docked in them.
  Moreover a new "View" menu item should exist, To allow toggling
  various windows.
- Give user color pickers for Normal, Specular, Roughness & Height Map
  so that they can manually paint those features & Allow preview in the
  editor?
  - https://youtu.be/gUkY8ZoRfuQ
- Allow user to generate a normal map?
  - https://youtu.be/-rJdOc9WZS4
- Give Plugins The Ability To Draw Text & Basic Shapes Instead Of Passing Them GUI?
  - A plugin that allows stop motion animation to be created using
    external connected camera.
- Convolutional Filters?
- Procedural Art? Like Math Based Art?
- Android Port (Meant for mainly tablets/big screen devices) Using
  [Rawdrawandroid](https://github.com/cnlohr/rawdrawandroid)

## References

Here are resources that have helped me while developing
this software.

- [It's probably time to stop recommending Clean Code](https://qntm.org/clean)
- [Enter The Arena: Simplifying Memory Management (2023)](https://youtu.be/TZ5a3gCCZYo)
- [Compositing and Blending Level 1](https://www.w3.org/TR/compositing-1/)
