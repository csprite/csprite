# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased](https://github.com/DEVLOPRR/CSprite/releases/tag/continuous)
- May 3, 2022
	- Remove UPX Compression From Linux Binary
	- Add Comments
	- Fix Flags
	- A Little Refactoring
	- Add Conditional Code For Debug & Release
	- Made The Code More Readable By Comments And Variable Names
	- Add Conditional Code For Windows Only
	- Remove Unused Headers

- May 1, 2022
	- Fix high CPU & GPU Usage ( May 1, 2022 )
	- Add Error Callback Function To Log GLFW Errors With Clear Description ( May 1, 2022 )
	- Fix Console Popping Up When Launching CSprite On Windows ( May 1, 2022 )

- April 30, 2022
	- Add Building For Windows ( April 30, 2022 )
	- Update Open/Save File Dialogs
		- Remove [ImGui File Dialogs](https://github.com/aiekick/ImGuiFileDialog) Because Of Compiling Errors on Windows.
		- Add [Tiny File Dialogs](https://sourceforge.net/projects/tinyfiledialogs/) Because Of Being Native And Less Code Required To Implement.
	- Add GLFW Compiled Library ( glfw3_mt.lib ) To Link With On Windows
	- Fix Random Pixels On Blank Canvas Because Of The Memory allocated not being empty.
	- Tweak Makefile For Conditional Flags depending on Platform.
