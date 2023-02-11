local ffi = require("ffi")
local c = ffi.C

ffi.cdef[[
	void Sys_OpenURL(const char* URL);
	void Show_OpenNewFileWindow(bool val);
	void Show_SaveAsFileWindow(bool val);
	void Show_PreferencesWindow(bool val);
	void Show_LayerRenameWindow(bool val);
	void Show_NewCanvasWindow(bool val);
	void WriteCanvasLayersToDisk();
	bool Csprite_CanUndo();
	bool Csprite_CanRedo();
	void Csprite_Undo();
	void Csprite_Redo();
	void Logger_Hide();
	void Logger_Show();
	void Logger_Draw(const char* title);
	bool Logger_IsHidden();

	typedef unsigned char palette_entry_t[4];
	typedef struct {
		char             name[512];
		char             author[523];
		unsigned int     numOfEntries;
		palette_entry_t*  Colors;
	} Palette_T;

	typedef struct {
		char name[512];
		char author[523];
		ImGuiStyle style;
	} theme_t;

	int32_t    GetPaletteArr_numOfEntries();
	int32_t    GetPaletteIndex();
	void       SetPaletteIndex(int32_t val);
	Palette_T* GetPaletteAtIndex(int32_t i);

	int32_t    GetThemeArr_numOfEntries();
	int32_t    GetThemeIndex();
	void       SetThemeIndex(int32_t val);
	theme_t*   GetThemeAtIndex(int32_t i);
]]

function _Error(err)
	io.stdout:write('Error: ' .. tostring(err) .. "\n");
	os.exit(1)
end

local function __OnImGui_Render()
	if c.igBeginMainMenuBar() then
		if (c.igBeginMenu("File", true)) then
			if c.igMenuItem_BoolPtr("New", nil, nil, true) then
				c.Show_NewCanvasWindow(true);
			end
			if c.igMenuItem_BoolPtr("Open", "Ctrl+O", nil, true) then
				c.Show_OpenNewFileWindow(true);
			end
			if c.igBeginMenu("Save", true) then
				if c.igMenuItem_BoolPtr("Save", "Ctrl+S", nil, true) then
					c.WriteCanvasLayersToDisk();
				end
				if c.igMenuItem_BoolPtr("Save As", "Ctrl+Shift+S", nil, true) then
					c.Show_SaveAsFileWindow(true);
				end
				c.igEndMenu();
			end
			c.igEndMenu();
		end
		if c.igBeginMenu("Edit", true) then
			if c.igMenuItem_Bool("Undo", "Ctrl+Z", false, c.Csprite_CanUndo()) then
				c.Csprite_Undo();
			end
			if c.igMenuItem_Bool("Redo", "Ctrl+Y", false, c.Csprite_CanRedo()) then
				c.Csprite_Redo();
			end
			if c.igBeginMenu("Palette", true) then
				local numOfEntries = c.GetPaletteArr_numOfEntries();
				for i = 0, numOfEntries - 1, 1 do
					local _palidx = c.GetPaletteIndex();
					local pal = c.GetPaletteAtIndex(i);
					if c.igMenuItem_BoolPtr(pal["name"], nil, nil, true) then
						c.SetPaletteIndex(i);
					end
					if c.igIsItemHovered(c.ImGuiHoveredFlags_AllowWhenDisabled) then
						c.igSetTooltip("%s", ffi.cast("char*", pal["author"]));
					end
					if _palidx == i then
						c.igSameLine(0.0, -1.0);
						c.igText("<");
					end
				end
				c.igEndMenu();
			end

			if c.igBeginMenu("Theme", true) then
				local numOfEntries = c.GetThemeArr_numOfEntries();
				for i = 0, numOfEntries - 1, 1 do
					local _themeidx = c.GetThemeIndex();
					local theme = c.GetThemeAtIndex(i);
					if c.igMenuItem_BoolPtr(theme["name"], nil, nil, true) then
						c.SetThemeIndex(i);
					end
					if c.igIsItemHovered(c.ImGuiHoveredFlags_AllowWhenDisabled) then
						c.igSetTooltip("%s", ffi.cast("char*", theme["author"]));
					end
					if _themeidx == i then
						c.igSameLine(0.0, -1.0);
						c.igText("<");
					end
				end
				c.igEndMenu();
			end

			if c.igMenuItem_BoolPtr("Preferences", nil, nil, true) then
				c.Show_PreferencesWindow(true);
			end

			c.igEndMenu();
		end
		if c.igBeginMenu("View", true) then
			if c.igMenuItem_BoolPtr("Logs", nil, nil, true) then
				if c.Logger_IsHidden() then
					c.Logger_Show();
				else
					c.Logger_Hide();
				end
			end
			c.igEndMenu();
		end
		if c.igBeginMenu("Help", true) then
			if c.igMenuItem_BoolPtr("Wiki", nil, nil, true) then
				c.Sys_OpenURL("https://csprite.github.io/wiki/");
			end
			if c.igMenuItem_BoolPtr("About", nil, nil, true) then
				c.Sys_OpenURL("https://github.com/pegvin/csprite/wiki/About-CSprite");
			end
			if c.igMenuItem_BoolPtr("GitHub", nil, nil, true) then
				c.Sys_OpenURL("https://github.com/pegvin/csprite");
			end
			c.igEndMenu();
		end
		c.igEndMainMenuBar();
	end
end

function _OnImGui_Render()
	xpcall(__OnImGui_Render, _Error)
end

