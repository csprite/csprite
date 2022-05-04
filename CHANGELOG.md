# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased](https://github.com/DEVLOPRR/CSprite/releases/tag/continuous)

- May 5, 2022
	- Fix Color not being selected when clicking on a color in Color Palette
	- Change Circle Brush To Default

- May 4, 2022

	- Add Ink Dropper Tool
	- Add Menu Bar & New Button To Create A new empty canvas [#4](https://github.com/DEVLOPRR/CSprite/pull/4)
	- Add New, Open, Save & Save As in Menu Bar [#4](https://github.com/DEVLOPRR/CSprite/pull/4)
	- Fix Memory Leaks
	- Remove Zoom Limit (Max Zoom Limit Now is `4294967295`)

- May 3, 2022
	- Add Simple Menu
	- Add `New`, `Open`, `Save` & `Save As` in Menu
	- Fix Memory Leaks

- May 2, 2022
	- Remove UPX Compression From Linux Binary
	- Add Comments
	- Fix Flags
	- A Little Refactoring
	- Add Conditional Code For Debug & Release
	- Made The Code More Readable By Comments And Variable Names
	- Add Conditional Code For Windows Only
	- Remove Unused Headers

- May 1, 2022
	- Fix high CPU & GPU Usage
	- Add Error Callback Function To Log GLFW Errors With Clear Description
	- Fix Console Popping Up When Launching CSprite On Windows

- April 30, 2022
	- Add Building For Windows
	- Update Open/Save File Dialogs
		- Remove [ImGui File Dialogs](https://github.com/aiekick/ImGuiFileDialog) Because Of Compiling Errors on Windows.
		- Add [Tiny File Dialogs](https://sourceforge.net/projects/tinyfiledialogs/) Because Of Being Native And Less Code Required To Implement.
	- Add GLFW Compiled Library ( glfw3_mt.lib ) To Link With On Windows
	- Fix Random Pixels On Blank Canvas Because Of The Memory allocated not being empty.
	- Tweak Makefile For Conditional Flags depending on Platform.
