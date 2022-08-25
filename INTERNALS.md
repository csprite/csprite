## CSprite internals
this document roughly explains about code of csprite & stuff, so that contributors or any code reader get's some idea.

---

## Code

most of the code of CSprite is C & C Styled & that's why i have used `malloc` & `free` instead of `new` & `delete`, i'm just using C++ for some stuff like strings etc & will completely switch to C in future because i didn't have much experience with strings in c back then.

you might've noticed that there is just a single executable with no-dlls & stuff that's because csprite embeds required stuff like fonts & icons, this is basically done in 2 parts, first you need to convert the assets to include-able c arrays which is done via the `tools/create_assets.py` python3 script, second you need a way to access the c-arrays which is basically done in `src/assets.c`, this is the file where our assets are included & this file provides functions to interact with the assets like: `assets_get` & `assets_list`

---
## Libraries

#### SDL2
csprite uses [SDL2](https://www.libsdl.org/) which basically handles window & rendering stuff to screen for us,
i'm using [SDL Renderer](https://wiki.libsdl.org/SDL_Renderer) which is basically a "wrapper" for multiple graphics APIs &
this way i can use Direct3d, Metal, OpenGL, Vulkan & Software depending on platform & user configuration.
This was very helpful as i am not really a graphics programmer.

#### ImGui
csprite uses [ImGui](https://github.com/ocornut/imgui/) v1.87 which is used for making GUIs, so i don't need to do that myself

#### STB
csprite uses [STB's](https://github.com/nothings/stb) Image Loader & Writer for Interacting With PNGs.

#### log.c & ini
csprite uses [Log.c](https://github.com/rxi/log.c) for logging stuff (i didn't want to re-invent the wheel?) &
[Ini](https://github.com/rxi/ini) for reading & writing csprite config.

#### Tiny File Dialogs
csprite uses [Tiny File Dialogs](https://sourceforge.net/projects/tinyfiledialogs/) for open & save dialogs,
tho i might re-write a file open & save dialog using ImGui because it takes some time to launch initially & using ImGui
i can control the theme too!

---

# Thanks
