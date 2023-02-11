local ffi = require("ffi")
local c = ffi.C

function _Error(err)
	io.stdout:write('Error: ' .. tostring(err) .. "\n");
	os.exit(1)
end

local boolPtr = ffi.typeof("bool[1]")
local isOpen = boolPtr();
isOpen[0] = false;

local function __OnImGui_Render()
	c.igBegin("Noice", isOpen, c.ImGuiWindowFlags_NoTitleBar + c.ImGuiWindowFlags_NoResize);
	c.igText("Hello World!");
	c.igEnd();
end

function _OnImGui_Render()
	xpcall(__OnImGui_Render, _Error)
end

