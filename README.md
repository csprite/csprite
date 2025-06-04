# csprite

> [!NOTE]
> _This README is intended as a technical overview of
> the software, Intended for developers. If you're an user, Please
> visit [csprite.github.io](https://csprite.github.io)_

The software currently only supports Linux, with plans for
expanding to Windows & Mac, & Possibly various other operating
systems as well.

You can download pre-built binaries of the software [here](https://github.com/csprite/csprite/actions/workflows/build.yml?query=branch%3Ac).

The main aim of this software is to be simple, Not only
in code but in terms of UX as well. This is why main
application code resides here, But various functionalities
like support for various file formats will be separated into
plugins.  This ensures that not only compiling the project
will be simpler but the overall foot print of the program
will be smaller as well & Only required things will be loaded.

## Todo
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
