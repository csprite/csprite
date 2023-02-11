local ffi = require("ffi")
local c = ffi.C

function _Start()
	ffi.cdef[[
typedef struct ImDrawChannel ImDrawChannel;
typedef struct ImDrawCmd ImDrawCmd;
typedef struct ImDrawData ImDrawData;
typedef struct ImDrawList ImDrawList;
typedef struct ImDrawListSharedData ImDrawListSharedData;
typedef struct ImDrawListSplitter ImDrawListSplitter;
typedef struct ImDrawVert ImDrawVert;
typedef struct ImFont ImFont;
typedef struct ImFontAtlas ImFontAtlas;
typedef struct ImFontBuilderIO ImFontBuilderIO;
typedef struct ImFontConfig ImFontConfig;
typedef struct ImFontGlyph ImFontGlyph;
typedef struct ImFontGlyphRangesBuilder ImFontGlyphRangesBuilder;
typedef struct ImColor ImColor;
typedef struct ImGuiContext ImGuiContext;
typedef struct ImGuiIO ImGuiIO;
typedef struct ImGuiInputTextCallbackData ImGuiInputTextCallbackData;
typedef struct ImGuiKeyData ImGuiKeyData;
typedef struct ImGuiListClipper ImGuiListClipper;
typedef struct ImGuiOnceUponAFrame ImGuiOnceUponAFrame;
typedef struct ImGuiPayload ImGuiPayload;
typedef struct ImGuiPlatformImeData ImGuiPlatformImeData;
typedef struct ImGuiSizeCallbackData ImGuiSizeCallbackData;
typedef struct ImGuiStorage ImGuiStorage;
typedef struct ImGuiStyle ImGuiStyle;
typedef struct ImGuiTableSortSpecs ImGuiTableSortSpecs;
typedef struct ImGuiTableColumnSortSpecs ImGuiTableColumnSortSpecs;
typedef struct ImGuiTextBuffer ImGuiTextBuffer;
typedef struct ImGuiTextFilter ImGuiTextFilter;
typedef struct ImGuiViewport ImGuiViewport;
struct ImDrawChannel;
struct ImDrawCmd;
struct ImDrawData;
struct ImDrawList;
struct ImDrawListSharedData;
struct ImDrawListSplitter;
struct ImDrawVert;
struct ImFont;
struct ImFontAtlas;
struct ImFontBuilderIO;
struct ImFontConfig;
struct ImFontGlyph;
struct ImFontGlyphRangesBuilder;
struct ImColor;
struct ImGuiContext;
struct ImGuiIO;
struct ImGuiInputTextCallbackData;
struct ImGuiKeyData;
struct ImGuiListClipper;
struct ImGuiOnceUponAFrame;
struct ImGuiPayload;
struct ImGuiPlatformImeData;
struct ImGuiSizeCallbackData;
struct ImGuiStorage;
struct ImGuiStyle;
struct ImGuiTableSortSpecs;
struct ImGuiTableColumnSortSpecs;
struct ImGuiTextBuffer;
struct ImGuiTextFilter;
struct ImGuiViewport;
typedef int ImGuiCol;
typedef int ImGuiCond;
typedef int ImGuiDataType;
typedef int ImGuiDir;
typedef int ImGuiMouseButton;
typedef int ImGuiMouseCursor;
typedef int ImGuiSortDirection;
typedef int ImGuiStyleVar;
typedef int ImGuiTableBgTarget;
typedef int ImDrawFlags;
typedef int ImDrawListFlags;
typedef int ImFontAtlasFlags;
typedef int ImGuiBackendFlags;
typedef int ImGuiButtonFlags;
typedef int ImGuiColorEditFlags;
typedef int ImGuiConfigFlags;
typedef int ImGuiComboFlags;
typedef int ImGuiDragDropFlags;
typedef int ImGuiFocusedFlags;
typedef int ImGuiHoveredFlags;
typedef int ImGuiInputTextFlags;
typedef int ImGuiKeyChord;
typedef int ImGuiPopupFlags;
typedef int ImGuiSelectableFlags;
typedef int ImGuiSliderFlags;
typedef int ImGuiTabBarFlags;
typedef int ImGuiTabItemFlags;
typedef int ImGuiTableFlags;
typedef int ImGuiTableColumnFlags;
typedef int ImGuiTableRowFlags;
typedef int ImGuiTreeNodeFlags;
typedef int ImGuiViewportFlags;
typedef int ImGuiWindowFlags;typedef void* ImTextureID;typedef unsigned short ImDrawIdx;
typedef unsigned int ImGuiID;
typedef signed char ImS8;
typedef unsigned char ImU8;
typedef signed short ImS16;
typedef unsigned short ImU16;
typedef signed int ImS32;
typedef unsigned int ImU32;
typedef signed long long ImS64;
typedef unsigned long long ImU64;
typedef unsigned short ImWchar16;
typedef unsigned int ImWchar32;typedef ImWchar16 ImWchar;
typedef int (*ImGuiInputTextCallback)(ImGuiInputTextCallbackData* data);
typedef void (*ImGuiSizeCallback)(ImGuiSizeCallbackData* data);
typedef void* (*ImGuiMemAllocFunc)(size_t sz, void* user_data);
typedef void (*ImGuiMemFreeFunc)(void* ptr, void* user_data);
typedef struct ImVec2 ImVec2;
struct ImVec2
{
    float x, y;
};
typedef struct ImVec4 ImVec4;
struct ImVec4
{
    float x, y, z, w;
};
typedef enum {
    ImGuiWindowFlags_None = 0,
    ImGuiWindowFlags_NoTitleBar = 1 << 0,
    ImGuiWindowFlags_NoResize = 1 << 1,
    ImGuiWindowFlags_NoMove = 1 << 2,
    ImGuiWindowFlags_NoScrollbar = 1 << 3,
    ImGuiWindowFlags_NoScrollWithMouse = 1 << 4,
    ImGuiWindowFlags_NoCollapse = 1 << 5,
    ImGuiWindowFlags_AlwaysAutoResize = 1 << 6,
    ImGuiWindowFlags_NoBackground = 1 << 7,
    ImGuiWindowFlags_NoSavedSettings = 1 << 8,
    ImGuiWindowFlags_NoMouseInputs = 1 << 9,
    ImGuiWindowFlags_MenuBar = 1 << 10,
    ImGuiWindowFlags_HorizontalScrollbar = 1 << 11,
    ImGuiWindowFlags_NoFocusOnAppearing = 1 << 12,
    ImGuiWindowFlags_NoBringToFrontOnFocus = 1 << 13,
    ImGuiWindowFlags_AlwaysVerticalScrollbar= 1 << 14,
    ImGuiWindowFlags_AlwaysHorizontalScrollbar=1<< 15,
    ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 16,
    ImGuiWindowFlags_NoNavInputs = 1 << 18,
    ImGuiWindowFlags_NoNavFocus = 1 << 19,
    ImGuiWindowFlags_UnsavedDocument = 1 << 20,
    ImGuiWindowFlags_NoNav = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus,
    ImGuiWindowFlags_NoDecoration = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse,
    ImGuiWindowFlags_NoInputs = ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus,
    ImGuiWindowFlags_NavFlattened = 1 << 23,
    ImGuiWindowFlags_ChildWindow = 1 << 24,
    ImGuiWindowFlags_Tooltip = 1 << 25,
    ImGuiWindowFlags_Popup = 1 << 26,
    ImGuiWindowFlags_Modal = 1 << 27,
    ImGuiWindowFlags_ChildMenu = 1 << 28,
}ImGuiWindowFlags_;
typedef enum {
    ImGuiInputTextFlags_None = 0,
    ImGuiInputTextFlags_CharsDecimal = 1 << 0,
    ImGuiInputTextFlags_CharsHexadecimal = 1 << 1,
    ImGuiInputTextFlags_CharsUppercase = 1 << 2,
    ImGuiInputTextFlags_CharsNoBlank = 1 << 3,
    ImGuiInputTextFlags_AutoSelectAll = 1 << 4,
    ImGuiInputTextFlags_EnterReturnsTrue = 1 << 5,
    ImGuiInputTextFlags_CallbackCompletion = 1 << 6,
    ImGuiInputTextFlags_CallbackHistory = 1 << 7,
    ImGuiInputTextFlags_CallbackAlways = 1 << 8,
    ImGuiInputTextFlags_CallbackCharFilter = 1 << 9,
    ImGuiInputTextFlags_AllowTabInput = 1 << 10,
    ImGuiInputTextFlags_CtrlEnterForNewLine = 1 << 11,
    ImGuiInputTextFlags_NoHorizontalScroll = 1 << 12,
    ImGuiInputTextFlags_AlwaysOverwrite = 1 << 13,
    ImGuiInputTextFlags_ReadOnly = 1 << 14,
    ImGuiInputTextFlags_Password = 1 << 15,
    ImGuiInputTextFlags_NoUndoRedo = 1 << 16,
    ImGuiInputTextFlags_CharsScientific = 1 << 17,
    ImGuiInputTextFlags_CallbackResize = 1 << 18,
    ImGuiInputTextFlags_CallbackEdit = 1 << 19,
    ImGuiInputTextFlags_EscapeClearsAll = 1 << 20,



}ImGuiInputTextFlags_;
typedef enum {
    ImGuiTreeNodeFlags_None = 0,
    ImGuiTreeNodeFlags_Selected = 1 << 0,
    ImGuiTreeNodeFlags_Framed = 1 << 1,
    ImGuiTreeNodeFlags_AllowItemOverlap = 1 << 2,
    ImGuiTreeNodeFlags_NoTreePushOnOpen = 1 << 3,
    ImGuiTreeNodeFlags_NoAutoOpenOnLog = 1 << 4,
    ImGuiTreeNodeFlags_DefaultOpen = 1 << 5,
    ImGuiTreeNodeFlags_OpenOnDoubleClick = 1 << 6,
    ImGuiTreeNodeFlags_OpenOnArrow = 1 << 7,
    ImGuiTreeNodeFlags_Leaf = 1 << 8,
    ImGuiTreeNodeFlags_Bullet = 1 << 9,
    ImGuiTreeNodeFlags_FramePadding = 1 << 10,
    ImGuiTreeNodeFlags_SpanAvailWidth = 1 << 11,
    ImGuiTreeNodeFlags_SpanFullWidth = 1 << 12,
    ImGuiTreeNodeFlags_NavLeftJumpsBackHere = 1 << 13,
    ImGuiTreeNodeFlags_CollapsingHeader = ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_NoAutoOpenOnLog,
}ImGuiTreeNodeFlags_;
typedef enum {
    ImGuiPopupFlags_None = 0,
    ImGuiPopupFlags_MouseButtonLeft = 0,
    ImGuiPopupFlags_MouseButtonRight = 1,
    ImGuiPopupFlags_MouseButtonMiddle = 2,
    ImGuiPopupFlags_MouseButtonMask_ = 0x1F,
    ImGuiPopupFlags_MouseButtonDefault_ = 1,
    ImGuiPopupFlags_NoOpenOverExistingPopup = 1 << 5,
    ImGuiPopupFlags_NoOpenOverItems = 1 << 6,
    ImGuiPopupFlags_AnyPopupId = 1 << 7,
    ImGuiPopupFlags_AnyPopupLevel = 1 << 8,
    ImGuiPopupFlags_AnyPopup = ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel,
}ImGuiPopupFlags_;
typedef enum {
    ImGuiSelectableFlags_None = 0,
    ImGuiSelectableFlags_DontClosePopups = 1 << 0,
    ImGuiSelectableFlags_SpanAllColumns = 1 << 1,
    ImGuiSelectableFlags_AllowDoubleClick = 1 << 2,
    ImGuiSelectableFlags_Disabled = 1 << 3,
    ImGuiSelectableFlags_AllowItemOverlap = 1 << 4,
}ImGuiSelectableFlags_;
typedef enum {
    ImGuiComboFlags_None = 0,
    ImGuiComboFlags_PopupAlignLeft = 1 << 0,
    ImGuiComboFlags_HeightSmall = 1 << 1,
    ImGuiComboFlags_HeightRegular = 1 << 2,
    ImGuiComboFlags_HeightLarge = 1 << 3,
    ImGuiComboFlags_HeightLargest = 1 << 4,
    ImGuiComboFlags_NoArrowButton = 1 << 5,
    ImGuiComboFlags_NoPreview = 1 << 6,
    ImGuiComboFlags_HeightMask_ = ImGuiComboFlags_HeightSmall | ImGuiComboFlags_HeightRegular | ImGuiComboFlags_HeightLarge | ImGuiComboFlags_HeightLargest,
}ImGuiComboFlags_;
typedef enum {
    ImGuiTabBarFlags_None = 0,
    ImGuiTabBarFlags_Reorderable = 1 << 0,
    ImGuiTabBarFlags_AutoSelectNewTabs = 1 << 1,
    ImGuiTabBarFlags_TabListPopupButton = 1 << 2,
    ImGuiTabBarFlags_NoCloseWithMiddleMouseButton = 1 << 3,
    ImGuiTabBarFlags_NoTabListScrollingButtons = 1 << 4,
    ImGuiTabBarFlags_NoTooltip = 1 << 5,
    ImGuiTabBarFlags_FittingPolicyResizeDown = 1 << 6,
    ImGuiTabBarFlags_FittingPolicyScroll = 1 << 7,
    ImGuiTabBarFlags_FittingPolicyMask_ = ImGuiTabBarFlags_FittingPolicyResizeDown | ImGuiTabBarFlags_FittingPolicyScroll,
    ImGuiTabBarFlags_FittingPolicyDefault_ = ImGuiTabBarFlags_FittingPolicyResizeDown,
}ImGuiTabBarFlags_;
typedef enum {
    ImGuiTabItemFlags_None = 0,
    ImGuiTabItemFlags_UnsavedDocument = 1 << 0,
    ImGuiTabItemFlags_SetSelected = 1 << 1,
    ImGuiTabItemFlags_NoCloseWithMiddleMouseButton = 1 << 2,
    ImGuiTabItemFlags_NoPushId = 1 << 3,
    ImGuiTabItemFlags_NoTooltip = 1 << 4,
    ImGuiTabItemFlags_NoReorder = 1 << 5,
    ImGuiTabItemFlags_Leading = 1 << 6,
    ImGuiTabItemFlags_Trailing = 1 << 7,
}ImGuiTabItemFlags_;
typedef enum {
    ImGuiTableFlags_None = 0,
    ImGuiTableFlags_Resizable = 1 << 0,
    ImGuiTableFlags_Reorderable = 1 << 1,
    ImGuiTableFlags_Hideable = 1 << 2,
    ImGuiTableFlags_Sortable = 1 << 3,
    ImGuiTableFlags_NoSavedSettings = 1 << 4,
    ImGuiTableFlags_ContextMenuInBody = 1 << 5,
    ImGuiTableFlags_RowBg = 1 << 6,
    ImGuiTableFlags_BordersInnerH = 1 << 7,
    ImGuiTableFlags_BordersOuterH = 1 << 8,
    ImGuiTableFlags_BordersInnerV = 1 << 9,
    ImGuiTableFlags_BordersOuterV = 1 << 10,
    ImGuiTableFlags_BordersH = ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH,
    ImGuiTableFlags_BordersV = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterV,
    ImGuiTableFlags_BordersInner = ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersInnerH,
    ImGuiTableFlags_BordersOuter = ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_BordersOuterH,
    ImGuiTableFlags_Borders = ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuter,
    ImGuiTableFlags_NoBordersInBody = 1 << 11,
    ImGuiTableFlags_NoBordersInBodyUntilResize = 1 << 12,
    ImGuiTableFlags_SizingFixedFit = 1 << 13,
    ImGuiTableFlags_SizingFixedSame = 2 << 13,
    ImGuiTableFlags_SizingStretchProp = 3 << 13,
    ImGuiTableFlags_SizingStretchSame = 4 << 13,
    ImGuiTableFlags_NoHostExtendX = 1 << 16,
    ImGuiTableFlags_NoHostExtendY = 1 << 17,
    ImGuiTableFlags_NoKeepColumnsVisible = 1 << 18,
    ImGuiTableFlags_PreciseWidths = 1 << 19,
    ImGuiTableFlags_NoClip = 1 << 20,
    ImGuiTableFlags_PadOuterX = 1 << 21,
    ImGuiTableFlags_NoPadOuterX = 1 << 22,
    ImGuiTableFlags_NoPadInnerX = 1 << 23,
    ImGuiTableFlags_ScrollX = 1 << 24,
    ImGuiTableFlags_ScrollY = 1 << 25,
    ImGuiTableFlags_SortMulti = 1 << 26,
    ImGuiTableFlags_SortTristate = 1 << 27,
    ImGuiTableFlags_SizingMask_ = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_SizingStretchSame,
}ImGuiTableFlags_;
typedef enum {
    ImGuiTableColumnFlags_None = 0,
    ImGuiTableColumnFlags_Disabled = 1 << 0,
    ImGuiTableColumnFlags_DefaultHide = 1 << 1,
    ImGuiTableColumnFlags_DefaultSort = 1 << 2,
    ImGuiTableColumnFlags_WidthStretch = 1 << 3,
    ImGuiTableColumnFlags_WidthFixed = 1 << 4,
    ImGuiTableColumnFlags_NoResize = 1 << 5,
    ImGuiTableColumnFlags_NoReorder = 1 << 6,
    ImGuiTableColumnFlags_NoHide = 1 << 7,
    ImGuiTableColumnFlags_NoClip = 1 << 8,
    ImGuiTableColumnFlags_NoSort = 1 << 9,
    ImGuiTableColumnFlags_NoSortAscending = 1 << 10,
    ImGuiTableColumnFlags_NoSortDescending = 1 << 11,
    ImGuiTableColumnFlags_NoHeaderLabel = 1 << 12,
    ImGuiTableColumnFlags_NoHeaderWidth = 1 << 13,
    ImGuiTableColumnFlags_PreferSortAscending = 1 << 14,
    ImGuiTableColumnFlags_PreferSortDescending = 1 << 15,
    ImGuiTableColumnFlags_IndentEnable = 1 << 16,
    ImGuiTableColumnFlags_IndentDisable = 1 << 17,
    ImGuiTableColumnFlags_IsEnabled = 1 << 24,
    ImGuiTableColumnFlags_IsVisible = 1 << 25,
    ImGuiTableColumnFlags_IsSorted = 1 << 26,
    ImGuiTableColumnFlags_IsHovered = 1 << 27,
    ImGuiTableColumnFlags_WidthMask_ = ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_WidthFixed,
    ImGuiTableColumnFlags_IndentMask_ = ImGuiTableColumnFlags_IndentEnable | ImGuiTableColumnFlags_IndentDisable,
    ImGuiTableColumnFlags_StatusMask_ = ImGuiTableColumnFlags_IsEnabled | ImGuiTableColumnFlags_IsVisible | ImGuiTableColumnFlags_IsSorted | ImGuiTableColumnFlags_IsHovered,
    ImGuiTableColumnFlags_NoDirectResize_ = 1 << 30,
}ImGuiTableColumnFlags_;
typedef enum {
    ImGuiTableRowFlags_None = 0,
    ImGuiTableRowFlags_Headers = 1 << 0,
}ImGuiTableRowFlags_;
typedef enum {
    ImGuiTableBgTarget_None = 0,
    ImGuiTableBgTarget_RowBg0 = 1,
    ImGuiTableBgTarget_RowBg1 = 2,
    ImGuiTableBgTarget_CellBg = 3,
}ImGuiTableBgTarget_;
typedef enum {
    ImGuiFocusedFlags_None = 0,
    ImGuiFocusedFlags_ChildWindows = 1 << 0,
    ImGuiFocusedFlags_RootWindow = 1 << 1,
    ImGuiFocusedFlags_AnyWindow = 1 << 2,
    ImGuiFocusedFlags_NoPopupHierarchy = 1 << 3,
    ImGuiFocusedFlags_RootAndChildWindows = ImGuiFocusedFlags_RootWindow | ImGuiFocusedFlags_ChildWindows,
}ImGuiFocusedFlags_;
typedef enum {
    ImGuiHoveredFlags_None = 0,
    ImGuiHoveredFlags_ChildWindows = 1 << 0,
    ImGuiHoveredFlags_RootWindow = 1 << 1,
    ImGuiHoveredFlags_AnyWindow = 1 << 2,
    ImGuiHoveredFlags_NoPopupHierarchy = 1 << 3,
    ImGuiHoveredFlags_AllowWhenBlockedByPopup = 1 << 5,
    ImGuiHoveredFlags_AllowWhenBlockedByActiveItem = 1 << 7,
    ImGuiHoveredFlags_AllowWhenOverlapped = 1 << 8,
    ImGuiHoveredFlags_AllowWhenDisabled = 1 << 9,
    ImGuiHoveredFlags_NoNavOverride = 1 << 10,
    ImGuiHoveredFlags_RectOnly = ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem | ImGuiHoveredFlags_AllowWhenOverlapped,
    ImGuiHoveredFlags_RootAndChildWindows = ImGuiHoveredFlags_RootWindow | ImGuiHoveredFlags_ChildWindows,
    ImGuiHoveredFlags_DelayNormal = 1 << 11,
    ImGuiHoveredFlags_DelayShort = 1 << 12,
    ImGuiHoveredFlags_NoSharedDelay = 1 << 13,
}ImGuiHoveredFlags_;
typedef enum {
    ImGuiDragDropFlags_None = 0,
    ImGuiDragDropFlags_SourceNoPreviewTooltip = 1 << 0,
    ImGuiDragDropFlags_SourceNoDisableHover = 1 << 1,
    ImGuiDragDropFlags_SourceNoHoldToOpenOthers = 1 << 2,
    ImGuiDragDropFlags_SourceAllowNullID = 1 << 3,
    ImGuiDragDropFlags_SourceExtern = 1 << 4,
    ImGuiDragDropFlags_SourceAutoExpirePayload = 1 << 5,
    ImGuiDragDropFlags_AcceptBeforeDelivery = 1 << 10,
    ImGuiDragDropFlags_AcceptNoDrawDefaultRect = 1 << 11,
    ImGuiDragDropFlags_AcceptNoPreviewTooltip = 1 << 12,
    ImGuiDragDropFlags_AcceptPeekOnly = ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect,
}ImGuiDragDropFlags_;
typedef enum {
    ImGuiDataType_S8,
    ImGuiDataType_U8,
    ImGuiDataType_S16,
    ImGuiDataType_U16,
    ImGuiDataType_S32,
    ImGuiDataType_U32,
    ImGuiDataType_S64,
    ImGuiDataType_U64,
    ImGuiDataType_Float,
    ImGuiDataType_Double,
    ImGuiDataType_COUNT
}ImGuiDataType_;
typedef enum {
    ImGuiDir_None = -1,
    ImGuiDir_Left = 0,
    ImGuiDir_Right = 1,
    ImGuiDir_Up = 2,
    ImGuiDir_Down = 3,
    ImGuiDir_COUNT
}ImGuiDir_;
typedef enum {
    ImGuiSortDirection_None = 0,
    ImGuiSortDirection_Ascending = 1,
    ImGuiSortDirection_Descending = 2
}ImGuiSortDirection_;
typedef enum {
ImGuiKey_None=0,
ImGuiKey_Tab=512,
ImGuiKey_LeftArrow=513,
ImGuiKey_RightArrow=514,
ImGuiKey_UpArrow=515,
ImGuiKey_DownArrow=516,
ImGuiKey_PageUp=517,
ImGuiKey_PageDown=518,
ImGuiKey_Home=519,
ImGuiKey_End=520,
ImGuiKey_Insert=521,
ImGuiKey_Delete=522,
ImGuiKey_Backspace=523,
ImGuiKey_Space=524,
ImGuiKey_Enter=525,
ImGuiKey_Escape=526,
ImGuiKey_LeftCtrl=527,
ImGuiKey_LeftShift=528,
ImGuiKey_LeftAlt=529,
ImGuiKey_LeftSuper=530,
ImGuiKey_RightCtrl=531,
ImGuiKey_RightShift=532,
ImGuiKey_RightAlt=533,
ImGuiKey_RightSuper=534,
ImGuiKey_Menu=535,
ImGuiKey_0=536,
ImGuiKey_1=537,
ImGuiKey_2=538,
ImGuiKey_3=539,
ImGuiKey_4=540,
ImGuiKey_5=541,
ImGuiKey_6=542,
ImGuiKey_7=543,
ImGuiKey_8=544,
ImGuiKey_9=545,
ImGuiKey_A=546,
ImGuiKey_B=547,
ImGuiKey_C=548,
ImGuiKey_D=549,
ImGuiKey_E=550,
ImGuiKey_F=551,
ImGuiKey_G=552,
ImGuiKey_H=553,
ImGuiKey_I=554,
ImGuiKey_J=555,
ImGuiKey_K=556,
ImGuiKey_L=557,
ImGuiKey_M=558,
ImGuiKey_N=559,
ImGuiKey_O=560,
ImGuiKey_P=561,
ImGuiKey_Q=562,
ImGuiKey_R=563,
ImGuiKey_S=564,
ImGuiKey_T=565,
ImGuiKey_U=566,
ImGuiKey_V=567,
ImGuiKey_W=568,
ImGuiKey_X=569,
ImGuiKey_Y=570,
ImGuiKey_Z=571,
ImGuiKey_F1=572,
ImGuiKey_F2=573,
ImGuiKey_F3=574,
ImGuiKey_F4=575,
ImGuiKey_F5=576,
ImGuiKey_F6=577,
ImGuiKey_F7=578,
ImGuiKey_F8=579,
ImGuiKey_F9=580,
ImGuiKey_F10=581,
ImGuiKey_F11=582,
ImGuiKey_F12=583,
ImGuiKey_Apostrophe=584,
ImGuiKey_Comma=585,
ImGuiKey_Minus=586,
ImGuiKey_Period=587,
ImGuiKey_Slash=588,
ImGuiKey_Semicolon=589,
ImGuiKey_Equal=590,
ImGuiKey_LeftBracket=591,
ImGuiKey_Backslash=592,
ImGuiKey_RightBracket=593,
ImGuiKey_GraveAccent=594,
ImGuiKey_CapsLock=595,
ImGuiKey_ScrollLock=596,
ImGuiKey_NumLock=597,
ImGuiKey_PrintScreen=598,
ImGuiKey_Pause=599,
ImGuiKey_Keypad0=600,
ImGuiKey_Keypad1=601,
ImGuiKey_Keypad2=602,
ImGuiKey_Keypad3=603,
ImGuiKey_Keypad4=604,
ImGuiKey_Keypad5=605,
ImGuiKey_Keypad6=606,
ImGuiKey_Keypad7=607,
ImGuiKey_Keypad8=608,
ImGuiKey_Keypad9=609,
ImGuiKey_KeypadDecimal=610,
ImGuiKey_KeypadDivide=611,
ImGuiKey_KeypadMultiply=612,
ImGuiKey_KeypadSubtract=613,
ImGuiKey_KeypadAdd=614,
ImGuiKey_KeypadEnter=615,
ImGuiKey_KeypadEqual=616,
ImGuiKey_GamepadStart=617,
ImGuiKey_GamepadBack=618,
ImGuiKey_GamepadFaceLeft=619,
ImGuiKey_GamepadFaceRight=620,
ImGuiKey_GamepadFaceUp=621,
ImGuiKey_GamepadFaceDown=622,
ImGuiKey_GamepadDpadLeft=623,
ImGuiKey_GamepadDpadRight=624,
ImGuiKey_GamepadDpadUp=625,
ImGuiKey_GamepadDpadDown=626,
ImGuiKey_GamepadL1=627,
ImGuiKey_GamepadR1=628,
ImGuiKey_GamepadL2=629,
ImGuiKey_GamepadR2=630,
ImGuiKey_GamepadL3=631,
ImGuiKey_GamepadR3=632,
ImGuiKey_GamepadLStickLeft=633,
ImGuiKey_GamepadLStickRight=634,
ImGuiKey_GamepadLStickUp=635,
ImGuiKey_GamepadLStickDown=636,
ImGuiKey_GamepadRStickLeft=637,
ImGuiKey_GamepadRStickRight=638,
ImGuiKey_GamepadRStickUp=639,
ImGuiKey_GamepadRStickDown=640,
ImGuiKey_MouseLeft=641,
ImGuiKey_MouseRight=642,
ImGuiKey_MouseMiddle=643,
ImGuiKey_MouseX1=644,
ImGuiKey_MouseX2=645,
ImGuiKey_MouseWheelX=646,
ImGuiKey_MouseWheelY=647,
ImGuiKey_ReservedForModCtrl=648,
ImGuiKey_ReservedForModShift=649,
ImGuiKey_ReservedForModAlt=650,
ImGuiKey_ReservedForModSuper=651,
ImGuiKey_COUNT=652,
ImGuiMod_None=0,
ImGuiMod_Ctrl=1 << 12,
ImGuiMod_Shift=1 << 13,
ImGuiMod_Alt=1 << 14,
ImGuiMod_Super=1 << 15,
ImGuiMod_Mask_=0xF000,
ImGuiMod_Shortcut=ImGuiMod_Ctrl,
ImGuiKey_NamedKey_BEGIN=512,
ImGuiKey_NamedKey_END=ImGuiKey_COUNT,
ImGuiKey_NamedKey_COUNT=ImGuiKey_NamedKey_END - ImGuiKey_NamedKey_BEGIN,
ImGuiKey_KeysData_SIZE=ImGuiKey_COUNT,
ImGuiKey_KeysData_OFFSET=0,
}ImGuiKey;
typedef enum {
    ImGuiNavInput_Activate, ImGuiNavInput_Cancel, ImGuiNavInput_Input, ImGuiNavInput_Menu, ImGuiNavInput_DpadLeft, ImGuiNavInput_DpadRight, ImGuiNavInput_DpadUp, ImGuiNavInput_DpadDown,
    ImGuiNavInput_LStickLeft, ImGuiNavInput_LStickRight, ImGuiNavInput_LStickUp, ImGuiNavInput_LStickDown, ImGuiNavInput_FocusPrev, ImGuiNavInput_FocusNext, ImGuiNavInput_TweakSlow, ImGuiNavInput_TweakFast,
    ImGuiNavInput_COUNT,
}ImGuiNavInput;
typedef enum {
    ImGuiConfigFlags_None = 0,
    ImGuiConfigFlags_NavEnableKeyboard = 1 << 0,
    ImGuiConfigFlags_NavEnableGamepad = 1 << 1,
    ImGuiConfigFlags_NavEnableSetMousePos = 1 << 2,
    ImGuiConfigFlags_NavNoCaptureKeyboard = 1 << 3,
    ImGuiConfigFlags_NoMouse = 1 << 4,
    ImGuiConfigFlags_NoMouseCursorChange = 1 << 5,
    ImGuiConfigFlags_IsSRGB = 1 << 20,
    ImGuiConfigFlags_IsTouchScreen = 1 << 21,
}ImGuiConfigFlags_;
typedef enum {
    ImGuiBackendFlags_None = 0,
    ImGuiBackendFlags_HasGamepad = 1 << 0,
    ImGuiBackendFlags_HasMouseCursors = 1 << 1,
    ImGuiBackendFlags_HasSetMousePos = 1 << 2,
    ImGuiBackendFlags_RendererHasVtxOffset = 1 << 3,
}ImGuiBackendFlags_;
typedef enum {
    ImGuiCol_Text,
    ImGuiCol_TextDisabled,
    ImGuiCol_WindowBg,
    ImGuiCol_ChildBg,
    ImGuiCol_PopupBg,
    ImGuiCol_Border,
    ImGuiCol_BorderShadow,
    ImGuiCol_FrameBg,
    ImGuiCol_FrameBgHovered,
    ImGuiCol_FrameBgActive,
    ImGuiCol_TitleBg,
    ImGuiCol_TitleBgActive,
    ImGuiCol_TitleBgCollapsed,
    ImGuiCol_MenuBarBg,
    ImGuiCol_ScrollbarBg,
    ImGuiCol_ScrollbarGrab,
    ImGuiCol_ScrollbarGrabHovered,
    ImGuiCol_ScrollbarGrabActive,
    ImGuiCol_CheckMark,
    ImGuiCol_SliderGrab,
    ImGuiCol_SliderGrabActive,
    ImGuiCol_Button,
    ImGuiCol_ButtonHovered,
    ImGuiCol_ButtonActive,
    ImGuiCol_Header,
    ImGuiCol_HeaderHovered,
    ImGuiCol_HeaderActive,
    ImGuiCol_Separator,
    ImGuiCol_SeparatorHovered,
    ImGuiCol_SeparatorActive,
    ImGuiCol_ResizeGrip,
    ImGuiCol_ResizeGripHovered,
    ImGuiCol_ResizeGripActive,
    ImGuiCol_Tab,
    ImGuiCol_TabHovered,
    ImGuiCol_TabActive,
    ImGuiCol_TabUnfocused,
    ImGuiCol_TabUnfocusedActive,
    ImGuiCol_PlotLines,
    ImGuiCol_PlotLinesHovered,
    ImGuiCol_PlotHistogram,
    ImGuiCol_PlotHistogramHovered,
    ImGuiCol_TableHeaderBg,
    ImGuiCol_TableBorderStrong,
    ImGuiCol_TableBorderLight,
    ImGuiCol_TableRowBg,
    ImGuiCol_TableRowBgAlt,
    ImGuiCol_TextSelectedBg,
    ImGuiCol_DragDropTarget,
    ImGuiCol_NavHighlight,
    ImGuiCol_NavWindowingHighlight,
    ImGuiCol_NavWindowingDimBg,
    ImGuiCol_ModalWindowDimBg,
    ImGuiCol_COUNT
}ImGuiCol_;
typedef enum {
    ImGuiStyleVar_Alpha,
    ImGuiStyleVar_DisabledAlpha,
    ImGuiStyleVar_WindowPadding,
    ImGuiStyleVar_WindowRounding,
    ImGuiStyleVar_WindowBorderSize,
    ImGuiStyleVar_WindowMinSize,
    ImGuiStyleVar_WindowTitleAlign,
    ImGuiStyleVar_ChildRounding,
    ImGuiStyleVar_ChildBorderSize,
    ImGuiStyleVar_PopupRounding,
    ImGuiStyleVar_PopupBorderSize,
    ImGuiStyleVar_FramePadding,
    ImGuiStyleVar_FrameRounding,
    ImGuiStyleVar_FrameBorderSize,
    ImGuiStyleVar_ItemSpacing,
    ImGuiStyleVar_ItemInnerSpacing,
    ImGuiStyleVar_IndentSpacing,
    ImGuiStyleVar_CellPadding,
    ImGuiStyleVar_ScrollbarSize,
    ImGuiStyleVar_ScrollbarRounding,
    ImGuiStyleVar_GrabMinSize,
    ImGuiStyleVar_GrabRounding,
    ImGuiStyleVar_TabRounding,
    ImGuiStyleVar_ButtonTextAlign,
    ImGuiStyleVar_SelectableTextAlign,
    ImGuiStyleVar_COUNT
}ImGuiStyleVar_;
typedef enum {
    ImGuiButtonFlags_None = 0,
    ImGuiButtonFlags_MouseButtonLeft = 1 << 0,
    ImGuiButtonFlags_MouseButtonRight = 1 << 1,
    ImGuiButtonFlags_MouseButtonMiddle = 1 << 2,
    ImGuiButtonFlags_MouseButtonMask_ = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle,
    ImGuiButtonFlags_MouseButtonDefault_ = ImGuiButtonFlags_MouseButtonLeft,
}ImGuiButtonFlags_;
typedef enum {
    ImGuiColorEditFlags_None = 0,
    ImGuiColorEditFlags_NoAlpha = 1 << 1,
    ImGuiColorEditFlags_NoPicker = 1 << 2,
    ImGuiColorEditFlags_NoOptions = 1 << 3,
    ImGuiColorEditFlags_NoSmallPreview = 1 << 4,
    ImGuiColorEditFlags_NoInputs = 1 << 5,
    ImGuiColorEditFlags_NoTooltip = 1 << 6,
    ImGuiColorEditFlags_NoLabel = 1 << 7,
    ImGuiColorEditFlags_NoSidePreview = 1 << 8,
    ImGuiColorEditFlags_NoDragDrop = 1 << 9,
    ImGuiColorEditFlags_NoBorder = 1 << 10,
    ImGuiColorEditFlags_AlphaBar = 1 << 16,
    ImGuiColorEditFlags_AlphaPreview = 1 << 17,
    ImGuiColorEditFlags_AlphaPreviewHalf= 1 << 18,
    ImGuiColorEditFlags_HDR = 1 << 19,
    ImGuiColorEditFlags_DisplayRGB = 1 << 20,
    ImGuiColorEditFlags_DisplayHSV = 1 << 21,
    ImGuiColorEditFlags_DisplayHex = 1 << 22,
    ImGuiColorEditFlags_Uint8 = 1 << 23,
    ImGuiColorEditFlags_Float = 1 << 24,
    ImGuiColorEditFlags_PickerHueBar = 1 << 25,
    ImGuiColorEditFlags_PickerHueWheel = 1 << 26,
    ImGuiColorEditFlags_InputRGB = 1 << 27,
    ImGuiColorEditFlags_InputHSV = 1 << 28,
    ImGuiColorEditFlags_DefaultOptions_ = ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_PickerHueBar,
    ImGuiColorEditFlags_DisplayMask_ = ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_DisplayHSV | ImGuiColorEditFlags_DisplayHex,
    ImGuiColorEditFlags_DataTypeMask_ = ImGuiColorEditFlags_Uint8 | ImGuiColorEditFlags_Float,
    ImGuiColorEditFlags_PickerMask_ = ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_PickerHueBar,
    ImGuiColorEditFlags_InputMask_ = ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_InputHSV,
}ImGuiColorEditFlags_;
typedef enum {
    ImGuiSliderFlags_None = 0,
    ImGuiSliderFlags_AlwaysClamp = 1 << 4,
    ImGuiSliderFlags_Logarithmic = 1 << 5,
    ImGuiSliderFlags_NoRoundToFormat = 1 << 6,
    ImGuiSliderFlags_NoInput = 1 << 7,
    ImGuiSliderFlags_InvalidMask_ = 0x7000000F,



}ImGuiSliderFlags_;
typedef enum {
    ImGuiMouseButton_Left = 0,
    ImGuiMouseButton_Right = 1,
    ImGuiMouseButton_Middle = 2,
    ImGuiMouseButton_COUNT = 5
}ImGuiMouseButton_;
typedef enum {
    ImGuiMouseCursor_None = -1,
    ImGuiMouseCursor_Arrow = 0,
    ImGuiMouseCursor_TextInput,
    ImGuiMouseCursor_ResizeAll,
    ImGuiMouseCursor_ResizeNS,
    ImGuiMouseCursor_ResizeEW,
    ImGuiMouseCursor_ResizeNESW,
    ImGuiMouseCursor_ResizeNWSE,
    ImGuiMouseCursor_Hand,
    ImGuiMouseCursor_NotAllowed,
    ImGuiMouseCursor_COUNT
}ImGuiMouseCursor_;
typedef enum {
    ImGuiCond_None = 0,
    ImGuiCond_Always = 1 << 0,
    ImGuiCond_Once = 1 << 1,
    ImGuiCond_FirstUseEver = 1 << 2,
    ImGuiCond_Appearing = 1 << 3,
}ImGuiCond_;
struct ImGuiStyle
{
    float Alpha;
    float DisabledAlpha;
    ImVec2 WindowPadding;
    float WindowRounding;
    float WindowBorderSize;
    ImVec2 WindowMinSize;
    ImVec2 WindowTitleAlign;
    ImGuiDir WindowMenuButtonPosition;
    float ChildRounding;
    float ChildBorderSize;
    float PopupRounding;
    float PopupBorderSize;
    ImVec2 FramePadding;
    float FrameRounding;
    float FrameBorderSize;
    ImVec2 ItemSpacing;
    ImVec2 ItemInnerSpacing;
    ImVec2 CellPadding;
    ImVec2 TouchExtraPadding;
    float IndentSpacing;
    float ColumnsMinSpacing;
    float ScrollbarSize;
    float ScrollbarRounding;
    float GrabMinSize;
    float GrabRounding;
    float LogSliderDeadzone;
    float TabRounding;
    float TabBorderSize;
    float TabMinWidthForCloseButton;
    ImGuiDir ColorButtonPosition;
    ImVec2 ButtonTextAlign;
    ImVec2 SelectableTextAlign;
    ImVec2 DisplayWindowPadding;
    ImVec2 DisplaySafeAreaPadding;
    float MouseCursorScale;
    bool AntiAliasedLines;
    bool AntiAliasedLinesUseTex;
    bool AntiAliasedFill;
    float CurveTessellationTol;
    float CircleTessellationMaxError;
    ImVec4 Colors[ImGuiCol_COUNT];
};
struct ImGuiKeyData
{
    bool Down;
    float DownDuration;
    float DownDurationPrev;
    float AnalogValue;
};
typedef struct ImVector_ImWchar {int Size;int Capacity;ImWchar* Data;} ImVector_ImWchar;

struct ImGuiIO
{    ImGuiConfigFlags ConfigFlags;
    ImGuiBackendFlags BackendFlags;
    ImVec2 DisplaySize;
    float DeltaTime;
    float IniSavingRate;
    const char* IniFilename;
    const char* LogFilename;
    float MouseDoubleClickTime;
    float MouseDoubleClickMaxDist;
    float MouseDragThreshold;
    float KeyRepeatDelay;
    float KeyRepeatRate;
    float HoverDelayNormal;
    float HoverDelayShort;
    void* UserData;    ImFontAtlas*Fonts;
    float FontGlobalScale;
    bool FontAllowUserScaling;
    ImFont* FontDefault;
    ImVec2 DisplayFramebufferScale;
    bool MouseDrawCursor;
    bool ConfigMacOSXBehaviors;
    bool ConfigInputTrickleEventQueue;
    bool ConfigInputTextCursorBlink;
    bool ConfigInputTextEnterKeepActive;
    bool ConfigDragClickToInputText;
    bool ConfigWindowsResizeFromEdges;
    bool ConfigWindowsMoveFromTitleBarOnly;
    float ConfigMemoryCompactTimer;
    const char* BackendPlatformName;
    const char* BackendRendererName;
    void* BackendPlatformUserData;
    void* BackendRendererUserData;
    void* BackendLanguageUserData;
    const char* (*GetClipboardTextFn)(void* user_data);
    void (*SetClipboardTextFn)(void* user_data, const char* text);
    void* ClipboardUserData;
    void (*SetPlatformImeDataFn)(ImGuiViewport* viewport, ImGuiPlatformImeData* data);    void* _UnusedPadding;    bool WantCaptureMouse;
    bool WantCaptureKeyboard;
    bool WantTextInput;
    bool WantSetMousePos;
    bool WantSaveIniSettings;
    bool NavActive;
    bool NavVisible;
    float Framerate;
    int MetricsRenderVertices;
    int MetricsRenderIndices;
    int MetricsRenderWindows;
    int MetricsActiveWindows;
    int MetricsActiveAllocations;
    ImVec2 MouseDelta;    int KeyMap[ImGuiKey_COUNT];
    bool KeysDown[ImGuiKey_COUNT];
    float NavInputs[ImGuiNavInput_COUNT];
    ImVec2 MousePos;
    bool MouseDown[5];
    float MouseWheel;
    float MouseWheelH;
    bool KeyCtrl;
    bool KeyShift;
    bool KeyAlt;
    bool KeySuper;
    ImGuiKeyChord KeyMods;
    ImGuiKeyData KeysData[ImGuiKey_KeysData_SIZE];
    bool WantCaptureMouseUnlessPopupClose;
    ImVec2 MousePosPrev;
    ImVec2 MouseClickedPos[5];
    double MouseClickedTime[5];
    bool MouseClicked[5];
    bool MouseDoubleClicked[5];
    ImU16 MouseClickedCount[5];
    ImU16 MouseClickedLastCount[5];
    bool MouseReleased[5];
    bool MouseDownOwned[5];
    bool MouseDownOwnedUnlessPopupClose[5];
    float MouseDownDuration[5];
    float MouseDownDurationPrev[5];
    float MouseDragMaxDistanceSqr[5];
    float PenPressure;
    bool AppFocusLost;
    bool AppAcceptingEvents;
    ImS8 BackendUsingLegacyKeyArrays;
    bool BackendUsingLegacyNavInputArray;
    ImWchar16 InputQueueSurrogate;
    ImVector_ImWchar InputQueueCharacters;
};
struct ImGuiInputTextCallbackData
{
    ImGuiInputTextFlags EventFlag;
    ImGuiInputTextFlags Flags;
    void* UserData;
    ImWchar EventChar;
    ImGuiKey EventKey;
    char* Buf;
    int BufTextLen;
    int BufSize;
    bool BufDirty;
    int CursorPos;
    int SelectionStart;
    int SelectionEnd;
};
struct ImGuiSizeCallbackData
{
    void* UserData;
    ImVec2 Pos;
    ImVec2 CurrentSize;
    ImVec2 DesiredSize;
};
struct ImGuiPayload
{
    void* Data;
    int DataSize;
    ImGuiID SourceId;
    ImGuiID SourceParentId;
    int DataFrameCount;
    char DataType[32 + 1];
    bool Preview;
    bool Delivery;
};
struct ImGuiTableColumnSortSpecs
{
    ImGuiID ColumnUserID;
    ImS16 ColumnIndex;
    ImS16 SortOrder;
    ImGuiSortDirection SortDirection : 8;
};
struct ImGuiTableSortSpecs
{
    const ImGuiTableColumnSortSpecs* Specs;
    int SpecsCount;
    bool SpecsDirty;
};
struct ImGuiOnceUponAFrame
{
     int RefFrame;
};
struct ImGuiTextRange
{
        const char* b;
        const char* e;
};
typedef struct ImGuiTextRange ImGuiTextRange;

typedef struct ImVector_ImGuiTextRange {int Size;int Capacity;ImGuiTextRange* Data;} ImVector_ImGuiTextRange;

struct ImGuiTextFilter
{
    char InputBuf[256];
    ImVector_ImGuiTextRange Filters;
    int CountGrep;
};
typedef struct ImGuiTextRange ImGuiTextRange;
typedef struct ImVector_char {int Size;int Capacity;char* Data;} ImVector_char;

struct ImGuiTextBuffer
{
    ImVector_char Buf;
};
struct ImGuiStoragePair
{
        ImGuiID key;
        union { int val_i; float val_f; void* val_p; };
};
typedef struct ImGuiStoragePair ImGuiStoragePair;

typedef struct ImVector_ImGuiStoragePair {int Size;int Capacity;ImGuiStoragePair* Data;} ImVector_ImGuiStoragePair;

struct ImGuiStorage
{    ImVector_ImGuiStoragePair Data;
};
typedef struct ImGuiStoragePair ImGuiStoragePair;
struct ImGuiListClipper
{
    int DisplayStart;
    int DisplayEnd;
    int ItemsCount;
    float ItemsHeight;
    float StartPosY;
    void* TempData;
};
struct ImColor
{
    ImVec4 Value;
};
typedef void (*ImDrawCallback)(const ImDrawList* parent_list, const ImDrawCmd* cmd);
struct ImDrawCmd
{
    ImVec4 ClipRect;
    ImTextureID TextureId;
    unsigned int VtxOffset;
    unsigned int IdxOffset;
    unsigned int ElemCount;
    ImDrawCallback UserCallback;
    void* UserCallbackData;
};
struct ImDrawVert
{
    ImVec2 pos;
    ImVec2 uv;
    ImU32 col;
};
typedef struct ImDrawCmdHeader ImDrawCmdHeader;
struct ImDrawCmdHeader
{
    ImVec4 ClipRect;
    ImTextureID TextureId;
    unsigned int VtxOffset;
};
typedef struct ImVector_ImDrawCmd {int Size;int Capacity;ImDrawCmd* Data;} ImVector_ImDrawCmd;

typedef struct ImVector_ImDrawIdx {int Size;int Capacity;ImDrawIdx* Data;} ImVector_ImDrawIdx;

struct ImDrawChannel
{
    ImVector_ImDrawCmd _CmdBuffer;
    ImVector_ImDrawIdx _IdxBuffer;
};
typedef struct ImVector_ImDrawChannel {int Size;int Capacity;ImDrawChannel* Data;} ImVector_ImDrawChannel;

struct ImDrawListSplitter
{
    int _Current;
    int _Count;
    ImVector_ImDrawChannel _Channels;
};
typedef enum {
    ImDrawFlags_None = 0,
    ImDrawFlags_Closed = 1 << 0,
    ImDrawFlags_RoundCornersTopLeft = 1 << 4,
    ImDrawFlags_RoundCornersTopRight = 1 << 5,
    ImDrawFlags_RoundCornersBottomLeft = 1 << 6,
    ImDrawFlags_RoundCornersBottomRight = 1 << 7,
    ImDrawFlags_RoundCornersNone = 1 << 8,
    ImDrawFlags_RoundCornersTop = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight,
    ImDrawFlags_RoundCornersBottom = ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight,
    ImDrawFlags_RoundCornersLeft = ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersTopLeft,
    ImDrawFlags_RoundCornersRight = ImDrawFlags_RoundCornersBottomRight | ImDrawFlags_RoundCornersTopRight,
    ImDrawFlags_RoundCornersAll = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight | ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight,
    ImDrawFlags_RoundCornersDefault_ = ImDrawFlags_RoundCornersAll,
    ImDrawFlags_RoundCornersMask_ = ImDrawFlags_RoundCornersAll | ImDrawFlags_RoundCornersNone,
}ImDrawFlags_;
typedef enum {
    ImDrawListFlags_None = 0,
    ImDrawListFlags_AntiAliasedLines = 1 << 0,
    ImDrawListFlags_AntiAliasedLinesUseTex = 1 << 1,
    ImDrawListFlags_AntiAliasedFill = 1 << 2,
    ImDrawListFlags_AllowVtxOffset = 1 << 3,
}ImDrawListFlags_;
typedef struct ImVector_ImDrawVert {int Size;int Capacity;ImDrawVert* Data;} ImVector_ImDrawVert;

typedef struct ImVector_ImVec4 {int Size;int Capacity;ImVec4* Data;} ImVector_ImVec4;

typedef struct ImVector_ImTextureID {int Size;int Capacity;ImTextureID* Data;} ImVector_ImTextureID;

typedef struct ImVector_ImVec2 {int Size;int Capacity;ImVec2* Data;} ImVector_ImVec2;

struct ImDrawList
{
    ImVector_ImDrawCmd CmdBuffer;
    ImVector_ImDrawIdx IdxBuffer;
    ImVector_ImDrawVert VtxBuffer;
    ImDrawListFlags Flags;
    unsigned int _VtxCurrentIdx;
    ImDrawListSharedData* _Data;
    const char* _OwnerName;
    ImDrawVert* _VtxWritePtr;
    ImDrawIdx* _IdxWritePtr;
    ImVector_ImVec4 _ClipRectStack;
    ImVector_ImTextureID _TextureIdStack;
    ImVector_ImVec2 _Path;
    ImDrawCmdHeader _CmdHeader;
    ImDrawListSplitter _Splitter;
    float _FringeScale;
};
struct ImDrawData
{
    bool Valid;
    int CmdListsCount;
    int TotalIdxCount;
    int TotalVtxCount;
    ImDrawList** CmdLists;
    ImVec2 DisplayPos;
    ImVec2 DisplaySize;
    ImVec2 FramebufferScale;
};
struct ImFontConfig
{
    void* FontData;
    int FontDataSize;
    bool FontDataOwnedByAtlas;
    int FontNo;
    float SizePixels;
    int OversampleH;
    int OversampleV;
    bool PixelSnapH;
    ImVec2 GlyphExtraSpacing;
    ImVec2 GlyphOffset;
    const ImWchar* GlyphRanges;
    float GlyphMinAdvanceX;
    float GlyphMaxAdvanceX;
    bool MergeMode;
    unsigned int FontBuilderFlags;
    float RasterizerMultiply;
    ImWchar EllipsisChar;
    char Name[40];
    ImFont* DstFont;
};
struct ImFontGlyph
{
    unsigned int Colored : 1;
    unsigned int Visible : 1;
    unsigned int Codepoint : 30;
    float AdvanceX;
    float X0, Y0, X1, Y1;
    float U0, V0, U1, V1;
};
typedef struct ImVector_ImU32 {int Size;int Capacity;ImU32* Data;} ImVector_ImU32;

struct ImFontGlyphRangesBuilder
{
    ImVector_ImU32 UsedChars;
};
typedef struct ImFontAtlasCustomRect ImFontAtlasCustomRect;
struct ImFontAtlasCustomRect
{
    unsigned short Width, Height;
    unsigned short X, Y;
    unsigned int GlyphID;
    float GlyphAdvanceX;
    ImVec2 GlyphOffset;
    ImFont* Font;
};
typedef enum {
    ImFontAtlasFlags_None = 0,
    ImFontAtlasFlags_NoPowerOfTwoHeight = 1 << 0,
    ImFontAtlasFlags_NoMouseCursors = 1 << 1,
    ImFontAtlasFlags_NoBakedLines = 1 << 2,
}ImFontAtlasFlags_;
typedef struct ImVector_ImFontPtr {int Size;int Capacity;ImFont** Data;} ImVector_ImFontPtr;

typedef struct ImVector_ImFontAtlasCustomRect {int Size;int Capacity;ImFontAtlasCustomRect* Data;} ImVector_ImFontAtlasCustomRect;

typedef struct ImVector_ImFontConfig {int Size;int Capacity;ImFontConfig* Data;} ImVector_ImFontConfig;

struct ImFontAtlas
{    ImFontAtlasFlags Flags;
    ImTextureID TexID;
    int TexDesiredWidth;
    int TexGlyphPadding;
    bool Locked;
    bool TexReady;
    bool TexPixelsUseColors;
    unsigned char* TexPixelsAlpha8;
    unsigned int* TexPixelsRGBA32;
    int TexWidth;
    int TexHeight;
    ImVec2 TexUvScale;
    ImVec2 TexUvWhitePixel;
    ImVector_ImFontPtr Fonts;
    ImVector_ImFontAtlasCustomRect CustomRects;
    ImVector_ImFontConfig ConfigData;
    ImVec4 TexUvLines[(63) + 1];
    const ImFontBuilderIO* FontBuilderIO;
    unsigned int FontBuilderFlags;
    int PackIdMouseCursors;
    int PackIdLines;
};
typedef struct ImVector_float {int Size;int Capacity;float* Data;} ImVector_float;

typedef struct ImVector_ImFontGlyph {int Size;int Capacity;ImFontGlyph* Data;} ImVector_ImFontGlyph;

struct ImFont
{
    ImVector_float IndexAdvanceX;
    float FallbackAdvanceX;
    float FontSize;
    ImVector_ImWchar IndexLookup;
    ImVector_ImFontGlyph Glyphs;
    const ImFontGlyph* FallbackGlyph;
    ImFontAtlas* ContainerAtlas;
    const ImFontConfig* ConfigData;
    short ConfigDataCount;
    ImWchar FallbackChar;
    ImWchar EllipsisChar;
    ImWchar DotChar;
    bool DirtyLookupTables;
    float Scale;
    float Ascent, Descent;
    int MetricsTotalSurface;
    ImU8 Used4kPagesMap[(0xFFFF +1)/4096/8];
};
typedef enum {
    ImGuiViewportFlags_None = 0,
    ImGuiViewportFlags_IsPlatformWindow = 1 << 0,
    ImGuiViewportFlags_IsPlatformMonitor = 1 << 1,
    ImGuiViewportFlags_OwnedByApp = 1 << 2,
}ImGuiViewportFlags_;
struct ImGuiViewport
{
    ImGuiViewportFlags Flags;
    ImVec2 Pos;
    ImVec2 Size;
    ImVec2 WorkPos;
    ImVec2 WorkSize;
    void* PlatformHandleRaw;
};
struct ImGuiPlatformImeData
{
    bool WantVisible;
    ImVec2 InputPos;
    float InputLineHeight;
};

ImVec2* ImVec2_ImVec2_Nil(void);
void ImVec2_destroy(ImVec2* self);
ImVec2* ImVec2_ImVec2_Float(float _x,float _y);
ImVec4* ImVec4_ImVec4_Nil(void);
void ImVec4_destroy(ImVec4* self);
ImVec4* ImVec4_ImVec4_Float(float _x,float _y,float _z,float _w);
ImGuiContext* igCreateContext(ImFontAtlas* shared_font_atlas);
void igDestroyContext(ImGuiContext* ctx);
ImGuiContext* igGetCurrentContext(void);
void igSetCurrentContext(ImGuiContext* ctx);
ImGuiIO* igGetIO(void);
ImGuiStyle* igGetStyle(void);
void igNewFrame(void);
void igEndFrame(void);
void igRender(void);
ImDrawData* igGetDrawData(void);
void igShowDemoWindow(bool* p_open);
void igShowMetricsWindow(bool* p_open);
void igShowDebugLogWindow(bool* p_open);
void igShowStackToolWindow(bool* p_open);
void igShowAboutWindow(bool* p_open);
void igShowStyleEditor(ImGuiStyle* ref);
bool igShowStyleSelector(const char* label);
void igShowFontSelector(const char* label);
void igShowUserGuide(void);
const char* igGetVersion(void);
void igStyleColorsDark(ImGuiStyle* dst);
void igStyleColorsLight(ImGuiStyle* dst);
void igStyleColorsClassic(ImGuiStyle* dst);
bool igBegin(const char* name,bool* p_open,ImGuiWindowFlags flags);
void igEnd(void);
bool igBeginChild_Str(const char* str_id,const ImVec2 size,bool border,ImGuiWindowFlags flags);
bool igBeginChild_ID(ImGuiID id,const ImVec2 size,bool border,ImGuiWindowFlags flags);
void igEndChild(void);
bool igIsWindowAppearing(void);
bool igIsWindowCollapsed(void);
bool igIsWindowFocused(ImGuiFocusedFlags flags);
bool igIsWindowHovered(ImGuiHoveredFlags flags);
ImDrawList* igGetWindowDrawList(void);
void igGetWindowPos(ImVec2 *pOut);
void igGetWindowSize(ImVec2 *pOut);
float igGetWindowWidth(void);
float igGetWindowHeight(void);
void igSetNextWindowPos(const ImVec2 pos,ImGuiCond cond,const ImVec2 pivot);
void igSetNextWindowSize(const ImVec2 size,ImGuiCond cond);
void igSetNextWindowSizeConstraints(const ImVec2 size_min,const ImVec2 size_max,ImGuiSizeCallback custom_callback,void* custom_callback_data);
void igSetNextWindowContentSize(const ImVec2 size);
void igSetNextWindowCollapsed(bool collapsed,ImGuiCond cond);
void igSetNextWindowFocus(void);
void igSetNextWindowScroll(const ImVec2 scroll);
void igSetNextWindowBgAlpha(float alpha);
void igSetWindowPos_Vec2(const ImVec2 pos,ImGuiCond cond);
void igSetWindowSize_Vec2(const ImVec2 size,ImGuiCond cond);
void igSetWindowCollapsed_Bool(bool collapsed,ImGuiCond cond);
void igSetWindowFocus_Nil(void);
void igSetWindowFontScale(float scale);
void igSetWindowPos_Str(const char* name,const ImVec2 pos,ImGuiCond cond);
void igSetWindowSize_Str(const char* name,const ImVec2 size,ImGuiCond cond);
void igSetWindowCollapsed_Str(const char* name,bool collapsed,ImGuiCond cond);
void igSetWindowFocus_Str(const char* name);
void igGetContentRegionAvail(ImVec2 *pOut);
void igGetContentRegionMax(ImVec2 *pOut);
void igGetWindowContentRegionMin(ImVec2 *pOut);
void igGetWindowContentRegionMax(ImVec2 *pOut);
float igGetScrollX(void);
float igGetScrollY(void);
void igSetScrollX(float scroll_x);
void igSetScrollY(float scroll_y);
float igGetScrollMaxX(void);
float igGetScrollMaxY(void);
void igSetScrollHereX(float center_x_ratio);
void igSetScrollHereY(float center_y_ratio);
void igSetScrollFromPosX(float local_x,float center_x_ratio);
void igSetScrollFromPosY(float local_y,float center_y_ratio);
void igPushFont(ImFont* font);
void igPopFont(void);
void igPushStyleColor_U32(ImGuiCol idx,ImU32 col);
void igPushStyleColor_Vec4(ImGuiCol idx,const ImVec4 col);
void igPopStyleColor(int count);
void igPushStyleVar_Float(ImGuiStyleVar idx,float val);
void igPushStyleVar_Vec2(ImGuiStyleVar idx,const ImVec2 val);
void igPopStyleVar(int count);
void igPushAllowKeyboardFocus(bool allow_keyboard_focus);
void igPopAllowKeyboardFocus(void);
void igPushButtonRepeat(bool repeat);
void igPopButtonRepeat(void);
void igPushItemWidth(float item_width);
void igPopItemWidth(void);
void igSetNextItemWidth(float item_width);
float igCalcItemWidth(void);
void igPushTextWrapPos(float wrap_local_pos_x);
void igPopTextWrapPos(void);
ImFont* igGetFont(void);
float igGetFontSize(void);
void igGetFontTexUvWhitePixel(ImVec2 *pOut);
ImU32 igGetColorU32_Col(ImGuiCol idx,float alpha_mul);
ImU32 igGetColorU32_Vec4(const ImVec4 col);
ImU32 igGetColorU32_U32(ImU32 col);
const ImVec4* igGetStyleColorVec4(ImGuiCol idx);
void igSeparator(void);
void igSameLine(float offset_from_start_x,float spacing);
void igNewLine(void);
void igSpacing(void);
void igDummy(const ImVec2 size);
void igIndent(float indent_w);
void igUnindent(float indent_w);
void igBeginGroup(void);
void igEndGroup(void);
void igGetCursorPos(ImVec2 *pOut);
float igGetCursorPosX(void);
float igGetCursorPosY(void);
void igSetCursorPos(const ImVec2 local_pos);
void igSetCursorPosX(float local_x);
void igSetCursorPosY(float local_y);
void igGetCursorStartPos(ImVec2 *pOut);
void igGetCursorScreenPos(ImVec2 *pOut);
void igSetCursorScreenPos(const ImVec2 pos);
void igAlignTextToFramePadding(void);
float igGetTextLineHeight(void);
float igGetTextLineHeightWithSpacing(void);
float igGetFrameHeight(void);
float igGetFrameHeightWithSpacing(void);
void igPushID_Str(const char* str_id);
void igPushID_StrStr(const char* str_id_begin,const char* str_id_end);
void igPushID_Ptr(const void* ptr_id);
void igPushID_Int(int int_id);
void igPopID(void);
ImGuiID igGetID_Str(const char* str_id);
ImGuiID igGetID_StrStr(const char* str_id_begin,const char* str_id_end);
ImGuiID igGetID_Ptr(const void* ptr_id);
void igTextUnformatted(const char* text,const char* text_end);
void igText(const char* fmt,...);
void igTextV(const char* fmt,va_list args);
void igTextColored(const ImVec4 col,const char* fmt,...);
void igTextColoredV(const ImVec4 col,const char* fmt,va_list args);
void igTextDisabled(const char* fmt,...);
void igTextDisabledV(const char* fmt,va_list args);
void igTextWrapped(const char* fmt,...);
void igTextWrappedV(const char* fmt,va_list args);
void igLabelText(const char* label,const char* fmt,...);
void igLabelTextV(const char* label,const char* fmt,va_list args);
void igBulletText(const char* fmt,...);
void igBulletTextV(const char* fmt,va_list args);
bool igButton(const char* label,const ImVec2 size);
bool igSmallButton(const char* label);
bool igInvisibleButton(const char* str_id,const ImVec2 size,ImGuiButtonFlags flags);
bool igArrowButton(const char* str_id,ImGuiDir dir);
bool igCheckbox(const char* label,bool* v);
bool igCheckboxFlags_IntPtr(const char* label,int* flags,int flags_value);
bool igCheckboxFlags_UintPtr(const char* label,unsigned int* flags,unsigned int flags_value);
bool igRadioButton_Bool(const char* label,bool active);
bool igRadioButton_IntPtr(const char* label,int* v,int v_button);
void igProgressBar(float fraction,const ImVec2 size_arg,const char* overlay);
void igBullet(void);
void igImage(ImTextureID user_texture_id,const ImVec2 size,const ImVec2 uv0,const ImVec2 uv1,const ImVec4 tint_col,const ImVec4 border_col);
bool igImageButton(const char* str_id,ImTextureID user_texture_id,const ImVec2 size,const ImVec2 uv0,const ImVec2 uv1,const ImVec4 bg_col,const ImVec4 tint_col);
bool igBeginCombo(const char* label,const char* preview_value,ImGuiComboFlags flags);
void igEndCombo(void);
bool igCombo_Str_arr(const char* label,int* current_item,const char* const items[],int items_count,int popup_max_height_in_items);
bool igCombo_Str(const char* label,int* current_item,const char* items_separated_by_zeros,int popup_max_height_in_items);
bool igCombo_FnBoolPtr(const char* label,int* current_item,bool(*items_getter)(void* data,int idx,const char** out_text),void* data,int items_count,int popup_max_height_in_items);
bool igDragFloat(const char* label,float* v,float v_speed,float v_min,float v_max,const char* format,ImGuiSliderFlags flags);
bool igDragFloat2(const char* label,float v[2],float v_speed,float v_min,float v_max,const char* format,ImGuiSliderFlags flags);
bool igDragFloat3(const char* label,float v[3],float v_speed,float v_min,float v_max,const char* format,ImGuiSliderFlags flags);
bool igDragFloat4(const char* label,float v[4],float v_speed,float v_min,float v_max,const char* format,ImGuiSliderFlags flags);
bool igDragFloatRange2(const char* label,float* v_current_min,float* v_current_max,float v_speed,float v_min,float v_max,const char* format,const char* format_max,ImGuiSliderFlags flags);
bool igDragInt(const char* label,int* v,float v_speed,int v_min,int v_max,const char* format,ImGuiSliderFlags flags);
bool igDragInt2(const char* label,int v[2],float v_speed,int v_min,int v_max,const char* format,ImGuiSliderFlags flags);
bool igDragInt3(const char* label,int v[3],float v_speed,int v_min,int v_max,const char* format,ImGuiSliderFlags flags);
bool igDragInt4(const char* label,int v[4],float v_speed,int v_min,int v_max,const char* format,ImGuiSliderFlags flags);
bool igDragIntRange2(const char* label,int* v_current_min,int* v_current_max,float v_speed,int v_min,int v_max,const char* format,const char* format_max,ImGuiSliderFlags flags);
bool igDragScalar(const char* label,ImGuiDataType data_type,void* p_data,float v_speed,const void* p_min,const void* p_max,const char* format,ImGuiSliderFlags flags);
bool igDragScalarN(const char* label,ImGuiDataType data_type,void* p_data,int components,float v_speed,const void* p_min,const void* p_max,const char* format,ImGuiSliderFlags flags);
bool igSliderFloat(const char* label,float* v,float v_min,float v_max,const char* format,ImGuiSliderFlags flags);
bool igSliderFloat2(const char* label,float v[2],float v_min,float v_max,const char* format,ImGuiSliderFlags flags);
bool igSliderFloat3(const char* label,float v[3],float v_min,float v_max,const char* format,ImGuiSliderFlags flags);
bool igSliderFloat4(const char* label,float v[4],float v_min,float v_max,const char* format,ImGuiSliderFlags flags);
bool igSliderAngle(const char* label,float* v_rad,float v_degrees_min,float v_degrees_max,const char* format,ImGuiSliderFlags flags);
bool igSliderInt(const char* label,int* v,int v_min,int v_max,const char* format,ImGuiSliderFlags flags);
bool igSliderInt2(const char* label,int v[2],int v_min,int v_max,const char* format,ImGuiSliderFlags flags);
bool igSliderInt3(const char* label,int v[3],int v_min,int v_max,const char* format,ImGuiSliderFlags flags);
bool igSliderInt4(const char* label,int v[4],int v_min,int v_max,const char* format,ImGuiSliderFlags flags);
bool igSliderScalar(const char* label,ImGuiDataType data_type,void* p_data,const void* p_min,const void* p_max,const char* format,ImGuiSliderFlags flags);
bool igSliderScalarN(const char* label,ImGuiDataType data_type,void* p_data,int components,const void* p_min,const void* p_max,const char* format,ImGuiSliderFlags flags);
bool igVSliderFloat(const char* label,const ImVec2 size,float* v,float v_min,float v_max,const char* format,ImGuiSliderFlags flags);
bool igVSliderInt(const char* label,const ImVec2 size,int* v,int v_min,int v_max,const char* format,ImGuiSliderFlags flags);
bool igVSliderScalar(const char* label,const ImVec2 size,ImGuiDataType data_type,void* p_data,const void* p_min,const void* p_max,const char* format,ImGuiSliderFlags flags);
bool igInputText(const char* label,char* buf,size_t buf_size,ImGuiInputTextFlags flags,ImGuiInputTextCallback callback,void* user_data);
bool igInputTextMultiline(const char* label,char* buf,size_t buf_size,const ImVec2 size,ImGuiInputTextFlags flags,ImGuiInputTextCallback callback,void* user_data);
bool igInputTextWithHint(const char* label,const char* hint,char* buf,size_t buf_size,ImGuiInputTextFlags flags,ImGuiInputTextCallback callback,void* user_data);
bool igInputFloat(const char* label,float* v,float step,float step_fast,const char* format,ImGuiInputTextFlags flags);
bool igInputFloat2(const char* label,float v[2],const char* format,ImGuiInputTextFlags flags);
bool igInputFloat3(const char* label,float v[3],const char* format,ImGuiInputTextFlags flags);
bool igInputFloat4(const char* label,float v[4],const char* format,ImGuiInputTextFlags flags);
bool igInputInt(const char* label,int* v,int step,int step_fast,ImGuiInputTextFlags flags);
bool igInputInt2(const char* label,int v[2],ImGuiInputTextFlags flags);
bool igInputInt3(const char* label,int v[3],ImGuiInputTextFlags flags);
bool igInputInt4(const char* label,int v[4],ImGuiInputTextFlags flags);
bool igInputDouble(const char* label,double* v,double step,double step_fast,const char* format,ImGuiInputTextFlags flags);
bool igInputScalar(const char* label,ImGuiDataType data_type,void* p_data,const void* p_step,const void* p_step_fast,const char* format,ImGuiInputTextFlags flags);
bool igInputScalarN(const char* label,ImGuiDataType data_type,void* p_data,int components,const void* p_step,const void* p_step_fast,const char* format,ImGuiInputTextFlags flags);
bool igColorEdit3(const char* label,float col[3],ImGuiColorEditFlags flags);
bool igColorEdit4(const char* label,float col[4],ImGuiColorEditFlags flags);
bool igColorPicker3(const char* label,float col[3],ImGuiColorEditFlags flags);
bool igColorPicker4(const char* label,float col[4],ImGuiColorEditFlags flags,const float* ref_col);
bool igColorButton(const char* desc_id,const ImVec4 col,ImGuiColorEditFlags flags,const ImVec2 size);
void igSetColorEditOptions(ImGuiColorEditFlags flags);
bool igTreeNode_Str(const char* label);
bool igTreeNode_StrStr(const char* str_id,const char* fmt,...);
bool igTreeNode_Ptr(const void* ptr_id,const char* fmt,...);
bool igTreeNodeV_Str(const char* str_id,const char* fmt,va_list args);
bool igTreeNodeV_Ptr(const void* ptr_id,const char* fmt,va_list args);
bool igTreeNodeEx_Str(const char* label,ImGuiTreeNodeFlags flags);
bool igTreeNodeEx_StrStr(const char* str_id,ImGuiTreeNodeFlags flags,const char* fmt,...);
bool igTreeNodeEx_Ptr(const void* ptr_id,ImGuiTreeNodeFlags flags,const char* fmt,...);
bool igTreeNodeExV_Str(const char* str_id,ImGuiTreeNodeFlags flags,const char* fmt,va_list args);
bool igTreeNodeExV_Ptr(const void* ptr_id,ImGuiTreeNodeFlags flags,const char* fmt,va_list args);
void igTreePush_Str(const char* str_id);
void igTreePush_Ptr(const void* ptr_id);
void igTreePop(void);
float igGetTreeNodeToLabelSpacing(void);
bool igCollapsingHeader_TreeNodeFlags(const char* label,ImGuiTreeNodeFlags flags);
bool igCollapsingHeader_BoolPtr(const char* label,bool* p_visible,ImGuiTreeNodeFlags flags);
void igSetNextItemOpen(bool is_open,ImGuiCond cond);
bool igSelectable_Bool(const char* label,bool selected,ImGuiSelectableFlags flags,const ImVec2 size);
bool igSelectable_BoolPtr(const char* label,bool* p_selected,ImGuiSelectableFlags flags,const ImVec2 size);
bool igBeginListBox(const char* label,const ImVec2 size);
void igEndListBox(void);
bool igListBox_Str_arr(const char* label,int* current_item,const char* const items[],int items_count,int height_in_items);
bool igListBox_FnBoolPtr(const char* label,int* current_item,bool(*items_getter)(void* data,int idx,const char** out_text),void* data,int items_count,int height_in_items);
void igPlotLines_FloatPtr(const char* label,const float* values,int values_count,int values_offset,const char* overlay_text,float scale_min,float scale_max,ImVec2 graph_size,int stride);
void igPlotLines_FnFloatPtr(const char* label,float(*values_getter)(void* data,int idx),void* data,int values_count,int values_offset,const char* overlay_text,float scale_min,float scale_max,ImVec2 graph_size);
void igPlotHistogram_FloatPtr(const char* label,const float* values,int values_count,int values_offset,const char* overlay_text,float scale_min,float scale_max,ImVec2 graph_size,int stride);
void igPlotHistogram_FnFloatPtr(const char* label,float(*values_getter)(void* data,int idx),void* data,int values_count,int values_offset,const char* overlay_text,float scale_min,float scale_max,ImVec2 graph_size);
void igValue_Bool(const char* prefix,bool b);
void igValue_Int(const char* prefix,int v);
void igValue_Uint(const char* prefix,unsigned int v);
void igValue_Float(const char* prefix,float v,const char* float_format);
bool igBeginMenuBar(void);
void igEndMenuBar(void);
bool igBeginMainMenuBar(void);
void igEndMainMenuBar(void);
bool igBeginMenu(const char* label,bool enabled);
void igEndMenu(void);
bool igMenuItem_Bool(const char* label,const char* shortcut,bool selected,bool enabled);
bool igMenuItem_BoolPtr(const char* label,const char* shortcut,bool* p_selected,bool enabled);
void igBeginTooltip(void);
void igEndTooltip(void);
void igSetTooltip(const char* fmt,...);
void igSetTooltipV(const char* fmt,va_list args);
bool igBeginPopup(const char* str_id,ImGuiWindowFlags flags);
bool igBeginPopupModal(const char* name,bool* p_open,ImGuiWindowFlags flags);
void igEndPopup(void);
void igOpenPopup_Str(const char* str_id,ImGuiPopupFlags popup_flags);
void igOpenPopup_ID(ImGuiID id,ImGuiPopupFlags popup_flags);
void igOpenPopupOnItemClick(const char* str_id,ImGuiPopupFlags popup_flags);
void igCloseCurrentPopup(void);
bool igBeginPopupContextItem(const char* str_id,ImGuiPopupFlags popup_flags);
bool igBeginPopupContextWindow(const char* str_id,ImGuiPopupFlags popup_flags);
bool igBeginPopupContextVoid(const char* str_id,ImGuiPopupFlags popup_flags);
bool igIsPopupOpen(const char* str_id,ImGuiPopupFlags flags);
bool igBeginTable(const char* str_id,int column,ImGuiTableFlags flags,const ImVec2 outer_size,float inner_width);
void igEndTable(void);
void igTableNextRow(ImGuiTableRowFlags row_flags,float min_row_height);
bool igTableNextColumn(void);
bool igTableSetColumnIndex(int column_n);
void igTableSetupColumn(const char* label,ImGuiTableColumnFlags flags,float init_width_or_weight,ImGuiID user_id);
void igTableSetupScrollFreeze(int cols,int rows);
void igTableHeadersRow(void);
void igTableHeader(const char* label);
ImGuiTableSortSpecs* igTableGetSortSpecs(void);
int igTableGetColumnCount(void);
int igTableGetColumnIndex(void);
int igTableGetRowIndex(void);
const char* igTableGetColumnName(int column_n);
ImGuiTableColumnFlags igTableGetColumnFlags(int column_n);
void igTableSetColumnEnabled(int column_n,bool v);
void igTableSetBgColor(ImGuiTableBgTarget target,ImU32 color,int column_n);
void igColumns(int count,const char* id,bool border);
void igNextColumn(void);
int igGetColumnIndex(void);
float igGetColumnWidth(int column_index);
void igSetColumnWidth(int column_index,float width);
float igGetColumnOffset(int column_index);
void igSetColumnOffset(int column_index,float offset_x);
int igGetColumnsCount(void);
bool igBeginTabBar(const char* str_id,ImGuiTabBarFlags flags);
void igEndTabBar(void);
bool igBeginTabItem(const char* label,bool* p_open,ImGuiTabItemFlags flags);
void igEndTabItem(void);
bool igTabItemButton(const char* label,ImGuiTabItemFlags flags);
void igSetTabItemClosed(const char* tab_or_docked_window_label);
void igLogToTTY(int auto_open_depth);
void igLogToFile(int auto_open_depth,const char* filename);
void igLogToClipboard(int auto_open_depth);
void igLogFinish(void);
void igLogButtons(void);
void igLogTextV(const char* fmt,va_list args);
bool igBeginDragDropSource(ImGuiDragDropFlags flags);
bool igSetDragDropPayload(const char* type,const void* data,size_t sz,ImGuiCond cond);
void igEndDragDropSource(void);
bool igBeginDragDropTarget(void);
const ImGuiPayload* igAcceptDragDropPayload(const char* type,ImGuiDragDropFlags flags);
void igEndDragDropTarget(void);
const ImGuiPayload* igGetDragDropPayload(void);
void igBeginDisabled(bool disabled);
void igEndDisabled(void);
void igPushClipRect(const ImVec2 clip_rect_min,const ImVec2 clip_rect_max,bool intersect_with_current_clip_rect);
void igPopClipRect(void);
void igSetItemDefaultFocus(void);
void igSetKeyboardFocusHere(int offset);
bool igIsItemHovered(ImGuiHoveredFlags flags);
bool igIsItemActive(void);
bool igIsItemFocused(void);
bool igIsItemClicked(ImGuiMouseButton mouse_button);
bool igIsItemVisible(void);
bool igIsItemEdited(void);
bool igIsItemActivated(void);
bool igIsItemDeactivated(void);
bool igIsItemDeactivatedAfterEdit(void);
bool igIsItemToggledOpen(void);
bool igIsAnyItemHovered(void);
bool igIsAnyItemActive(void);
bool igIsAnyItemFocused(void);
void igGetItemRectMin(ImVec2 *pOut);
void igGetItemRectMax(ImVec2 *pOut);
void igGetItemRectSize(ImVec2 *pOut);
void igSetItemAllowOverlap(void);
ImGuiViewport* igGetMainViewport(void);
ImDrawList* igGetBackgroundDrawList(void);
ImDrawList* igGetForegroundDrawList(void);
bool igIsRectVisible_Nil(const ImVec2 size);
bool igIsRectVisible_Vec2(const ImVec2 rect_min,const ImVec2 rect_max);
double igGetTime(void);
int igGetFrameCount(void);
ImDrawListSharedData* igGetDrawListSharedData(void);
const char* igGetStyleColorName(ImGuiCol idx);
void igSetStateStorage(ImGuiStorage* storage);
ImGuiStorage* igGetStateStorage(void);
bool igBeginChildFrame(ImGuiID id,const ImVec2 size,ImGuiWindowFlags flags);
void igEndChildFrame(void);
void igCalcTextSize(ImVec2 *pOut,const char* text,const char* text_end,bool hide_text_after_double_hash,float wrap_width);
void igColorConvertU32ToFloat4(ImVec4 *pOut,ImU32 in);
ImU32 igColorConvertFloat4ToU32(const ImVec4 in);
void igColorConvertRGBtoHSV(float r,float g,float b,float* out_h,float* out_s,float* out_v);
void igColorConvertHSVtoRGB(float h,float s,float v,float* out_r,float* out_g,float* out_b);
bool igIsKeyDown(ImGuiKey key);
bool igIsKeyPressed(ImGuiKey key,bool repeat);
bool igIsKeyReleased(ImGuiKey key);
int igGetKeyPressedAmount(ImGuiKey key,float repeat_delay,float rate);
const char* igGetKeyName(ImGuiKey key);
void igSetNextFrameWantCaptureKeyboard(bool want_capture_keyboard);
bool igIsMouseDown(ImGuiMouseButton button);
bool igIsMouseClicked(ImGuiMouseButton button,bool repeat);
bool igIsMouseReleased(ImGuiMouseButton button);
bool igIsMouseDoubleClicked(ImGuiMouseButton button);
int igGetMouseClickedCount(ImGuiMouseButton button);
bool igIsMouseHoveringRect(const ImVec2 r_min,const ImVec2 r_max,bool clip);
bool igIsMousePosValid(const ImVec2* mouse_pos);
bool igIsAnyMouseDown(void);
void igGetMousePos(ImVec2 *pOut);
void igGetMousePosOnOpeningCurrentPopup(ImVec2 *pOut);
bool igIsMouseDragging(ImGuiMouseButton button,float lock_threshold);
void igGetMouseDragDelta(ImVec2 *pOut,ImGuiMouseButton button,float lock_threshold);
void igResetMouseDragDelta(ImGuiMouseButton button);
ImGuiMouseCursor igGetMouseCursor(void);
void igSetMouseCursor(ImGuiMouseCursor cursor_type);
void igSetNextFrameWantCaptureMouse(bool want_capture_mouse);
const char* igGetClipboardText(void);
void igSetClipboardText(const char* text);
void igLoadIniSettingsFromDisk(const char* ini_filename);
void igLoadIniSettingsFromMemory(const char* ini_data,size_t ini_size);
void igSaveIniSettingsToDisk(const char* ini_filename);
const char* igSaveIniSettingsToMemory(size_t* out_ini_size);
void igDebugTextEncoding(const char* text);
bool igDebugCheckVersionAndDataLayout(const char* version_str,size_t sz_io,size_t sz_style,size_t sz_vec2,size_t sz_vec4,size_t sz_drawvert,size_t sz_drawidx);
void igSetAllocatorFunctions(ImGuiMemAllocFunc alloc_func,ImGuiMemFreeFunc free_func,void* user_data);
void igGetAllocatorFunctions(ImGuiMemAllocFunc* p_alloc_func,ImGuiMemFreeFunc* p_free_func,void** p_user_data);
void* igMemAlloc(size_t size);
void igMemFree(void* ptr);
ImGuiStyle* ImGuiStyle_ImGuiStyle(void);
void ImGuiStyle_destroy(ImGuiStyle* self);
void ImGuiStyle_ScaleAllSizes(ImGuiStyle* self,float scale_factor);
void ImGuiIO_AddKeyEvent(ImGuiIO* self,ImGuiKey key,bool down);
void ImGuiIO_AddKeyAnalogEvent(ImGuiIO* self,ImGuiKey key,bool down,float v);
void ImGuiIO_AddMousePosEvent(ImGuiIO* self,float x,float y);
void ImGuiIO_AddMouseButtonEvent(ImGuiIO* self,int button,bool down);
void ImGuiIO_AddMouseWheelEvent(ImGuiIO* self,float wh_x,float wh_y);
void ImGuiIO_AddFocusEvent(ImGuiIO* self,bool focused);
void ImGuiIO_AddInputCharacter(ImGuiIO* self,unsigned int c);
void ImGuiIO_AddInputCharacterUTF16(ImGuiIO* self,ImWchar16 c);
void ImGuiIO_AddInputCharactersUTF8(ImGuiIO* self,const char* str);
void ImGuiIO_SetKeyEventNativeData(ImGuiIO* self,ImGuiKey key,int native_keycode,int native_scancode,int native_legacy_index);
void ImGuiIO_SetAppAcceptingEvents(ImGuiIO* self,bool accepting_events);
void ImGuiIO_ClearInputCharacters(ImGuiIO* self);
void ImGuiIO_ClearInputKeys(ImGuiIO* self);
ImGuiIO* ImGuiIO_ImGuiIO(void);
void ImGuiIO_destroy(ImGuiIO* self);
ImGuiInputTextCallbackData* ImGuiInputTextCallbackData_ImGuiInputTextCallbackData(void);
void ImGuiInputTextCallbackData_destroy(ImGuiInputTextCallbackData* self);
void ImGuiInputTextCallbackData_DeleteChars(ImGuiInputTextCallbackData* self,int pos,int bytes_count);
void ImGuiInputTextCallbackData_InsertChars(ImGuiInputTextCallbackData* self,int pos,const char* text,const char* text_end);
void ImGuiInputTextCallbackData_SelectAll(ImGuiInputTextCallbackData* self);
void ImGuiInputTextCallbackData_ClearSelection(ImGuiInputTextCallbackData* self);
bool ImGuiInputTextCallbackData_HasSelection(ImGuiInputTextCallbackData* self);
ImGuiPayload* ImGuiPayload_ImGuiPayload(void);
void ImGuiPayload_destroy(ImGuiPayload* self);
void ImGuiPayload_Clear(ImGuiPayload* self);
bool ImGuiPayload_IsDataType(ImGuiPayload* self,const char* type);
bool ImGuiPayload_IsPreview(ImGuiPayload* self);
bool ImGuiPayload_IsDelivery(ImGuiPayload* self);
ImGuiTableColumnSortSpecs* ImGuiTableColumnSortSpecs_ImGuiTableColumnSortSpecs(void);
void ImGuiTableColumnSortSpecs_destroy(ImGuiTableColumnSortSpecs* self);
ImGuiTableSortSpecs* ImGuiTableSortSpecs_ImGuiTableSortSpecs(void);
void ImGuiTableSortSpecs_destroy(ImGuiTableSortSpecs* self);
ImGuiOnceUponAFrame* ImGuiOnceUponAFrame_ImGuiOnceUponAFrame(void);
void ImGuiOnceUponAFrame_destroy(ImGuiOnceUponAFrame* self);
ImGuiTextFilter* ImGuiTextFilter_ImGuiTextFilter(const char* default_filter);
void ImGuiTextFilter_destroy(ImGuiTextFilter* self);
bool ImGuiTextFilter_Draw(ImGuiTextFilter* self,const char* label,float width);
bool ImGuiTextFilter_PassFilter(ImGuiTextFilter* self,const char* text,const char* text_end);
void ImGuiTextFilter_Build(ImGuiTextFilter* self);
void ImGuiTextFilter_Clear(ImGuiTextFilter* self);
bool ImGuiTextFilter_IsActive(ImGuiTextFilter* self);
ImGuiTextRange* ImGuiTextRange_ImGuiTextRange_Nil(void);
void ImGuiTextRange_destroy(ImGuiTextRange* self);
ImGuiTextRange* ImGuiTextRange_ImGuiTextRange_Str(const char* _b,const char* _e);
bool ImGuiTextRange_empty(ImGuiTextRange* self);
void ImGuiTextRange_split(ImGuiTextRange* self,char separator,ImVector_ImGuiTextRange* out);
ImGuiTextBuffer* ImGuiTextBuffer_ImGuiTextBuffer(void);
void ImGuiTextBuffer_destroy(ImGuiTextBuffer* self);
const char* ImGuiTextBuffer_begin(ImGuiTextBuffer* self);
const char* ImGuiTextBuffer_end(ImGuiTextBuffer* self);
int ImGuiTextBuffer_size(ImGuiTextBuffer* self);
bool ImGuiTextBuffer_empty(ImGuiTextBuffer* self);
void ImGuiTextBuffer_clear(ImGuiTextBuffer* self);
void ImGuiTextBuffer_reserve(ImGuiTextBuffer* self,int capacity);
const char* ImGuiTextBuffer_c_str(ImGuiTextBuffer* self);
void ImGuiTextBuffer_append(ImGuiTextBuffer* self,const char* str,const char* str_end);
void ImGuiTextBuffer_appendfv(ImGuiTextBuffer* self,const char* fmt,va_list args);
ImGuiStoragePair* ImGuiStoragePair_ImGuiStoragePair_Int(ImGuiID _key,int _val_i);
void ImGuiStoragePair_destroy(ImGuiStoragePair* self);
ImGuiStoragePair* ImGuiStoragePair_ImGuiStoragePair_Float(ImGuiID _key,float _val_f);
ImGuiStoragePair* ImGuiStoragePair_ImGuiStoragePair_Ptr(ImGuiID _key,void* _val_p);
void ImGuiStorage_Clear(ImGuiStorage* self);
int ImGuiStorage_GetInt(ImGuiStorage* self,ImGuiID key,int default_val);
void ImGuiStorage_SetInt(ImGuiStorage* self,ImGuiID key,int val);
bool ImGuiStorage_GetBool(ImGuiStorage* self,ImGuiID key,bool default_val);
void ImGuiStorage_SetBool(ImGuiStorage* self,ImGuiID key,bool val);
float ImGuiStorage_GetFloat(ImGuiStorage* self,ImGuiID key,float default_val);
void ImGuiStorage_SetFloat(ImGuiStorage* self,ImGuiID key,float val);
void* ImGuiStorage_GetVoidPtr(ImGuiStorage* self,ImGuiID key);
void ImGuiStorage_SetVoidPtr(ImGuiStorage* self,ImGuiID key,void* val);
int* ImGuiStorage_GetIntRef(ImGuiStorage* self,ImGuiID key,int default_val);
bool* ImGuiStorage_GetBoolRef(ImGuiStorage* self,ImGuiID key,bool default_val);
float* ImGuiStorage_GetFloatRef(ImGuiStorage* self,ImGuiID key,float default_val);
void** ImGuiStorage_GetVoidPtrRef(ImGuiStorage* self,ImGuiID key,void* default_val);
void ImGuiStorage_SetAllInt(ImGuiStorage* self,int val);
void ImGuiStorage_BuildSortByKey(ImGuiStorage* self);
ImGuiListClipper* ImGuiListClipper_ImGuiListClipper(void);
void ImGuiListClipper_destroy(ImGuiListClipper* self);
void ImGuiListClipper_Begin(ImGuiListClipper* self,int items_count,float items_height);
void ImGuiListClipper_End(ImGuiListClipper* self);
bool ImGuiListClipper_Step(ImGuiListClipper* self);
void ImGuiListClipper_ForceDisplayRangeByIndices(ImGuiListClipper* self,int item_min,int item_max);
ImColor* ImColor_ImColor_Nil(void);
void ImColor_destroy(ImColor* self);
ImColor* ImColor_ImColor_Float(float r,float g,float b,float a);
ImColor* ImColor_ImColor_Vec4(const ImVec4 col);
ImColor* ImColor_ImColor_Int(int r,int g,int b,int a);
ImColor* ImColor_ImColor_U32(ImU32 rgba);
void ImColor_SetHSV(ImColor* self,float h,float s,float v,float a);
void ImColor_HSV(ImColor *pOut,float h,float s,float v,float a);

	]]
end

function _Error(err)
	io.stdout:write('Error: ' .. tostring(err) .. "\n");
	os.exit(1)
end

xpcall(_Start, _Error)

