# csprite
A simple pixel art editor

![Csprite UI Preview](https://csprite.github.io/assets/csprite-preview.png)

---
### Features
- :beginner: Simple & intuitive
- :zap: Extremely fast
- :package: Small (Just ~2 MB)
- :desktop_computer: Cross-platform
- :moneybag: Free & open-source

### Requirements
- 50MB of Usable RAM
- OpenGL 3.0 or Higher
- Windows, Linux & Mac (for Others See [Building From Source](https://csprite.github.io/wiki/latest-git/building-from-source.html))

### Download
- You can get csprite from [downloads page](https://csprite.github.io/downloads/) or you can build it from source from the instructions [here](https://csprite.github.io/wiki/latest-git/building-from-source.html)

### Controls
| Key                          | Description                  |
|------------------------------|------------------------------|
| B                            | Circle Brush                 |
| Shift + B                    | Square Brush                 |
| E                            | Circle Eraser                |
| Shift + E                    | Square Eraser                |
| I                            | Color Picker/Ink Dropper     |
| Plus/Minus                   | Brush Size Increase/Decrease |
| Space + Mouse Drag           | Pan                          |
| Left Mouse                   | Draw/Erase                   |
| Ctrl + Plus / Scroll Up      | Zoom In                      |
| Ctrl + Minus / Scroll Down   | Zoom Out                     |

### Contributing
If you would like to help, [checkout the guide on contributing](https://github.com/csprite/.github/blob/master/CONTRIBUTING.md)

### Credits
- The beautiful icon & banner are created ["Birds Probably" aka Lilith](https://www.instagram.com/birds_probably/), Show the bird some love
- [Sweetie 16](https://lospec.com/palette-list/sweetie-16) Palette By [GrafxKid](http://grafxkid.tumblr.com/palettes).
- [Noto Sans Mono](https://fonts.google.com/noto/specimen/Noto+Sans+Mono) UI Font.

### Todo
- [x] A Simple UI & Menu
- [x] Support For Windows
- [x] Square/Circle in Brush/Eraser
- [x] Adjustable Brush/Eraser Size
- [ ] Undo Redo
- [x] Ink Dropper Tool
- [ ] Rectangle Tool
- [ ] Line Tool
- [ ] Circle Tool
- [ ] Configurable Option To Let Choose How Brush/Eye-Dropper Work
  - For Eye Dropper, Users can either select the pixel value in that layer or the composited/blended pixel value
  - For Brush Tool, Users can either replace the pixel on that layer or they can draw and the color will be blended with it.
- [ ] Flood/Bucket Fill Tool (https://lodev.org/cgtutor/floodfill.html#Scanline_Floodfill_Algorithm_With_Stack)
- [ ] https://lodev.org/cgtutor/painting.html
- [ ] [Porter Duff's Alpha Functions](https://www.pismin.com/10.1145/800031.808606)
- [ ] Line Tool (https://lodev.org/cgtutor/lineclipping.html)
- [ ] Color Blending
  - [x] Alpha
  - [x] Addition
  - [x] Subtraction
  - [x] Difference
  - [x] Multiply
  - [x] Screen
  - [ ] Overlay
  - [x] Darken
  - [x] Lighten
  - [ ] Color Dodge/Burn
  - [ ] Hard/Soft Light

### References/Resources
list of references & resources i used while developing csprite, might be good for learning.

- Simple Alpha-Blending: https://stackoverflow.com/a/68936281/14516016
- Talk/Coding live stream on color blending by TSoding: https://youtu.be/aKnl8-qifvE
- Re-Orderable list in ImGui using Drag & Drop API: https://github.com/ocornut/imgui/issues/1931#issuecomment-403305945
- LuaJIT in Csprite: https://github.com/pegvin/csprite/tree/luajit
- Compositing and Blending Level 1: https://www.w3.org/TR/compositing-1/

---
## Thanks
