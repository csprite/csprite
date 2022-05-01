# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased](https://github.com/DEVLOPRR/CSprite/releases/tag/continuous)
- Add Building For Windows ( April 30, 2022 )
  - Update Open/Save File Dialogs
    - Remove [ImGui File Dialogs](https://github.com/aiekick/ImGuiFileDialog) Because Of Compiling Errors on Windows.
    - Add [Tiny File Dialogs](https://sourceforge.net/projects/tinyfiledialogs/) Because Of Being Native And Less Code Required To Implement.
  - Add GLFW Compiled Library ( glfw3_mt.lib ) To Link With On Windows
  - Fix Random Pixels On Blank Canvas Because Of The Memory allocated not being empty.
  - Tweak Makefile For Conditional Flags depending on Platform.
