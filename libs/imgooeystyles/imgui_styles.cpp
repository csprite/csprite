#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "imgui_styles.h"

// Related To The Ini Library I've Embedded In This File (Scroll Down)
#define INI_VERSION "0.1.1"
typedef struct ini_t ini_t;

ini_t*      ini_load(const char *filename);
ini_t*      ini_load_txt(const char *iniTxt);
void        ini_free(ini_t *ini);
const char* ini_get(ini_t *ini, const char *section, const char *key);
int         ini_sget(ini_t *ini, const char *section, const char *key, const char *scanfmt, void *dst);

/* Case insensitive string compare upto n */
static int strncmpci(const char* s1, const char* s2, size_t n) {
	while (n && *s1 && ( tolower(*s1) == tolower(*s2) )) {
		++s1;
		++s2;
		--n;
	}
	if (n == 0) return 0;
	else return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
}

static const char* ImGuiDirToText(int direction) {
	switch (direction) {
		case ImGuiDir_None:   return "None";
		case ImGuiDir_Left:   return "Left";
		case ImGuiDir_Right:  return "Right";
		case ImGuiDir_Up:     return "Up";
		case ImGuiDir_Down:   return "Down";
		default:              return "Invalid";
	}
}

static int ImGuiTextToDir(const char* direction) {
	if (direction == NULL) return -1;
	if (strncmpci(direction, "None", 4) == 0)       return ImGuiDir_None;
	else if (strncmpci(direction, "Left", 4) == 0)  return ImGuiDir_Left;
	else if (strncmpci(direction, "Right", 5) == 0) return ImGuiDir_Right;
	else if (strncmpci(direction, "Up", 2) == 0)    return ImGuiDir_Up;
	else if (strncmpci(direction, "Down", 4) == 0)  return ImGuiDir_Down;
	else return -1;
}

IMGUI_API void ImGui::SaveStylesTo(const char* fileName) {
	ImGuiStyle& style = ImGui::GetStyle();
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL) return;

	// Header
	fprintf(fp, "[ImGuiStyles]\n");

	#define WRITE_FLOAT(name) fprintf(fp, #name" = %f\n", style.name)
	#define WRITE_ImVec2(name) fprintf(fp, #name" = %f,%f\n", style.name.x, style.name.y);
	#define WRITE_DIRECTION(name) fprintf(fp, #name" = %s\n", ImGuiDirToText(style.name));
	#define WRITE_BOOLEAN(name) fprintf(fp, #name" = %s\n", style.name == true ? "true" : "false");

	// Floats
	WRITE_FLOAT(Alpha);
	WRITE_FLOAT(DisabledAlpha);
	WRITE_FLOAT(WindowRounding);
	WRITE_FLOAT(WindowBorderSize);
	WRITE_FLOAT(ChildRounding);
	WRITE_FLOAT(ChildBorderSize);
	WRITE_FLOAT(PopupRounding);
	WRITE_FLOAT(PopupBorderSize);
	WRITE_FLOAT(FrameRounding);
	WRITE_FLOAT(FrameBorderSize);
	WRITE_FLOAT(IndentSpacing);
	WRITE_FLOAT(ColumnsMinSpacing);
	WRITE_FLOAT(ScrollbarSize);
	WRITE_FLOAT(ScrollbarRounding);
	WRITE_FLOAT(GrabMinSize);
	WRITE_FLOAT(GrabRounding);
	WRITE_FLOAT(LogSliderDeadzone);
	WRITE_FLOAT(TabRounding);
	WRITE_FLOAT(TabBorderSize);
	WRITE_FLOAT(TabMinWidthForCloseButton);
	WRITE_FLOAT(MouseCursorScale);
	WRITE_FLOAT(CurveTessellationTol);
	WRITE_FLOAT(CircleTessellationMaxError);

	// Directions
	WRITE_DIRECTION(WindowMenuButtonPosition);
	WRITE_DIRECTION(ColorButtonPosition);

	// Booleans
	WRITE_BOOLEAN(AntiAliasedLines);
	WRITE_BOOLEAN(AntiAliasedLinesUseTex);
	WRITE_BOOLEAN(AntiAliasedFill);

	// ImVec2s
	WRITE_ImVec2(WindowPadding);
	WRITE_ImVec2(WindowMinSize);
	WRITE_ImVec2(WindowTitleAlign);
	WRITE_ImVec2(FramePadding);
	WRITE_ImVec2(ItemSpacing);
	WRITE_ImVec2(ItemInnerSpacing);
	WRITE_ImVec2(CellPadding);
	WRITE_ImVec2(TouchExtraPadding);
	WRITE_ImVec2(ButtonTextAlign);
	WRITE_ImVec2(SelectableTextAlign);
	WRITE_ImVec2(DisplayWindowPadding);
	WRITE_ImVec2(DisplaySafeAreaPadding);

	fprintf(fp, "\n[ImGuiColors]\n");
	for (int i = 0; i < ImGuiCol_COUNT; i++) {
		const char* name = ImGui::GetStyleColorName(i);
		int color[4] = {
			ImClamp(IM_F32_TO_INT8_UNBOUND(style.Colors[i].x), 0, 255),
			ImClamp(IM_F32_TO_INT8_UNBOUND(style.Colors[i].y), 0, 255),
			ImClamp(IM_F32_TO_INT8_UNBOUND(style.Colors[i].z), 0, 255),
			ImClamp(IM_F32_TO_INT8_UNBOUND(style.Colors[i].w), 0, 255)
		};
		fprintf(fp, "%s = #%02X%02X%02X%02X\n", name, color[0], color[1], color[2], color[3]);
	}

	fclose(fp);
	fp = NULL;
}


IMGUI_API void ImGui::LoadStyleFrom(const char* fileName, ImGuiStyle& style) {
	ini_t* ini_style = ini_load(fileName);
	if (ini_style == NULL) return;
	// if (style == NULL) return;

	#define LOAD_FLOAT(name) { \
		const char* Str_##name  = ini_get(ini_style, "ImGuiStyles", #name); \
		style.name = Str_##name != NULL ? atof(Str_##name) : style.name; \
	}

	// Floats
	LOAD_FLOAT(Alpha);
	LOAD_FLOAT(DisabledAlpha);
	LOAD_FLOAT(WindowRounding);
	LOAD_FLOAT(WindowBorderSize);
	LOAD_FLOAT(ChildRounding);
	LOAD_FLOAT(ChildBorderSize);
	LOAD_FLOAT(PopupRounding);
	LOAD_FLOAT(PopupBorderSize);
	LOAD_FLOAT(FrameRounding);
	LOAD_FLOAT(FrameBorderSize);
	LOAD_FLOAT(IndentSpacing);
	LOAD_FLOAT(ColumnsMinSpacing);
	LOAD_FLOAT(ScrollbarSize);
	LOAD_FLOAT(ScrollbarRounding);
	LOAD_FLOAT(GrabMinSize);
	LOAD_FLOAT(GrabRounding);
	LOAD_FLOAT(LogSliderDeadzone);
	LOAD_FLOAT(TabRounding);
	LOAD_FLOAT(TabBorderSize);
	LOAD_FLOAT(TabMinWidthForCloseButton);
	LOAD_FLOAT(MouseCursorScale);
	LOAD_FLOAT(CurveTessellationTol);
	LOAD_FLOAT(CircleTessellationMaxError);

	// Directions
	#define LOAD_DIRECTION(name) { \
		const char* str  = ini_get(ini_style, "ImGuiStyles", #name); \
		if (str != NULL) { \
			int direction = ImGuiTextToDir(str); \
			if (direction >= 0) style.name = direction; \
		} \
	}
	LOAD_DIRECTION(WindowMenuButtonPosition);
	LOAD_DIRECTION(ColorButtonPosition);

	// Booleans
	#define LOAD_BOOLEANS(name) { \
		const char* str  = ini_get(ini_style, "ImGuiStyles", #name); \
		if (str != NULL) { \
			if (strncmpci(str, "true", 4) == 0) style.name = true; \
			else if (strncmpci(str, "false", 5) == 0) style.name = false; \
		} \
	}
	LOAD_BOOLEANS(AntiAliasedLines);
	LOAD_BOOLEANS(AntiAliasedLinesUseTex);
	LOAD_BOOLEANS(AntiAliasedFill);

	#define MAX_BEFORE_COMMA_SIZE 512
	char* CommaOffset = NULL;
	char BeforeComma[MAX_BEFORE_COMMA_SIZE] = "";

	#define LOAD_IMVEC2s(var_imvec2, name) { \
		const char* str = ini_get(ini_style, "ImGuiStyles", #name); \
		CommaOffset = str == NULL ? NULL : strchr(str, ','); \
		if (CommaOffset != NULL && CommaOffset - str > 0) { \
			strncpy(BeforeComma, str, CommaOffset - str); \
			BeforeComma[(CommaOffset - str) + 1] = '\0'; \
			var_imvec2.x = atof(BeforeComma); \
			var_imvec2.y = atof(CommaOffset + 1); \
		} \
		BeforeComma[0] = '\0'; \
		CommaOffset = NULL; \
	}

	// ImVec2s
	LOAD_IMVEC2s(style.WindowPadding, WindowPadding);
	LOAD_IMVEC2s(style.WindowMinSize, WindowMinSize);
	LOAD_IMVEC2s(style.WindowTitleAlign, WindowTitleAlign);
	LOAD_IMVEC2s(style.FramePadding, FramePadding);
	LOAD_IMVEC2s(style.ItemSpacing, ItemSpacing);
	LOAD_IMVEC2s(style.ItemInnerSpacing, ItemInnerSpacing);
	LOAD_IMVEC2s(style.CellPadding, CellPadding);
	LOAD_IMVEC2s(style.TouchExtraPadding, TouchExtraPadding);
	LOAD_IMVEC2s(style.ButtonTextAlign, ButtonTextAlign);
	LOAD_IMVEC2s(style.SelectableTextAlign, SelectableTextAlign);
	LOAD_IMVEC2s(style.DisplayWindowPadding, DisplayWindowPadding);
	LOAD_IMVEC2s(style.DisplaySafeAreaPadding, DisplaySafeAreaPadding);

	for (int i = 0; i < ImGuiCol_COUNT; i++) {
		const char* name = ImGui::GetStyleColorName(i);
		const char* value = ini_get(ini_style, "ImGuiColors", name);
		if (value != NULL) {
			unsigned int color[4] = { 0, 0, 0, 0 };
			if (sscanf(value[0] == '#' ? value + 1 : value, "%02x%02x%02x%02x", &color[0], &color[1], &color[2], &color[3]) == 4) {
				style.Colors[i].x = (float)color[0] / 255;
				style.Colors[i].y = (float)color[1] / 255;
				style.Colors[i].z = (float)color[2] / 255;
				style.Colors[i].w = (float)color[3] / 255;
			}
		}
	}

	ini_free(ini_style);
	ini_style = NULL;
}

// All The Code From Here Is Modified Version Of https://github.com/rxi/ini
// It's Packed Into 1 File For "Ease" Of Use

/**
 * Copyright (c) 2016 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

struct ini_t {
	char *data;
	char *end;
};

/* Case insensitive string compare */
static int strcmpci(const char *a, const char *b) {
	for (;;) {
	int d = tolower(*a) - tolower(*b);
	if (d != 0 || !*a) {
		return d;
	}
	a++, b++;
	}
}

/* Returns the next string in the split data */
static char* next(ini_t *ini, char *p) {
	p += strlen(p);
	while (p < ini->end && *p == '\0') {
	p++;
	}
	return p;
}

static void trim_back(ini_t *ini, char *p) {
	while (p >= ini->data && (*p == ' ' || *p == '\t' || *p == '\r')) {
	*p-- = '\0';
	}
}

static char* discard_line(ini_t *ini, char *p) {
	while (p < ini->end && *p != '\n') {
	*p++ = '\0';
	}
	return p;
}


static char *unescape_quoted_value(ini_t *ini, char *p) {
	/* Use `q` as write-head and `p` as read-head, `p` is always ahead of `q`
	 * as escape sequences are always larger than their resultant data */
	char *q = p;
	p++;
	while (p < ini->end && *p != '"' && *p != '\r' && *p != '\n') {
	if (*p == '\\') {
		/* Handle escaped char */
		p++;
		switch (*p) {
		default   : *q = *p;    break;
		case 'r'  : *q = '\r';  break;
		case 'n'  : *q = '\n';  break;
		case 't'  : *q = '\t';  break;
		case '\r' :
		case '\n' :
		case '\0' : goto end;
		}

	} else {
		/* Handle normal char */
		*q = *p;
	}
	q++, p++;
	}
end:
	return q;
}


/* Splits data in place into strings containing section-headers, keys and
 * values using one or more '\0' as a delimiter. Unescapes quoted values */
static void split_data(ini_t *ini) {
	char *value_start, *line_start;
	char *p = ini->data;

	while (p < ini->end) {
	switch (*p) {
		case '\r':
		case '\n':
		case '\t':
		case ' ':
		*p = '\0';
		/* Fall through */

		case '\0':
		p++;
		break;

		case '[':
		p += strcspn(p, "]\n");
		*p = '\0';
		break;

		case ';':
		p = discard_line(ini, p);
		break;

		default:
		line_start = p;
		p += strcspn(p, "=\n");

		/* Is line missing a '='? */
		if (*p != '=') {
			p = discard_line(ini, line_start);
			break;
		}
		trim_back(ini, p - 1);

		/* Replace '=' and whitespace after it with '\0' */
		do {
			*p++ = '\0';
		} while (*p == ' ' || *p == '\r' || *p == '\t');

		/* Is a value after '=' missing? */
		if (*p == '\n' || *p == '\0') {
			p = discard_line(ini, line_start);
			break;
		}

		if (*p == '"') {
			/* Handle quoted string value */
			value_start = p;
			p = unescape_quoted_value(ini, p);

			/* Was the string empty? */
			if (p == value_start) {
			p = discard_line(ini, line_start);
			break;
			}

			/* Discard the rest of the line after the string value */
			p = discard_line(ini, p);

		} else {
			/* Handle normal value */
			p += strcspn(p, "\n");
			trim_back(ini, p - 1);
		}
		break;
	}
	}
}


ini_t* ini_load_txt(const char *iniTxt) {
	ini_t *ini = NULL;
	int len = 0;

	if (iniTxt == NULL) goto fail;
	len = strlen(iniTxt);

	/* Init ini struct */
	ini = (ini_t*)malloc(sizeof(ini_t));
	if (!ini) goto fail;
	memset(ini, 0, sizeof(*ini));

	/* Load file content into memory, null terminate, init end var */
	ini->data = (char*)malloc(len + 1);
	ini->data[len] = '\0';
	ini->end = ini->data + len;
	strncpy(ini->data, iniTxt, len);

	/* Prepare data */
	split_data(ini);

	return ini;

fail:
	if (ini) ini_free(ini);
	return NULL;
}

ini_t* ini_load(const char *filename) {
	ini_t *ini = NULL;
	FILE *fp = NULL;
	int n, sz;

	/* Init ini struct */
	ini = (ini_t*)malloc(sizeof(ini_t));
	if (!ini) {
	goto fail;
	}
	memset(ini, 0, sizeof(ini_t));

	/* Open file */
	fp = fopen(filename, "rb");
	if (!fp) {
	goto fail;
	}

	/* Get file size */
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	rewind(fp);

	/* Load file content into memory, null terminate, init end var */
	ini->data = (char*)malloc(sz + 1);
	ini->data[sz] = '\0';
	ini->end = ini->data  + sz;
	n = fread(ini->data, 1, sz, fp);
	if (n != sz) {
	goto fail;
	}

	/* Prepare data */
	split_data(ini);

	/* Clean up and return */
	fclose(fp);
	return ini;

fail:
	if (fp) fclose(fp);
	if (ini) ini_free(ini);
	return NULL;
}


void ini_free(ini_t *ini) {
	free(ini->data);
	free(ini);
}


const char* ini_get(ini_t *ini, const char *section, const char *key) {
	if (ini == NULL) {
	return NULL;
	}

	char *current_section = NULL;
	char *val;
	char *p = ini->data;

	if (*p == '\0') {
	p = next(ini, p);
	}

	while (p < ini->end) {
	if (*p == '[') {
		/* Handle section */
		current_section = p + 1;

	} else {
		/* Handle key */
		val = next(ini, p);
		if (!section || !strcmpci(section, current_section)) {
		if (!strcmpci(p, key)) {
			return val;
		}
		}
		p = val;
	}

	p = next(ini, p);
	}

	return NULL;
}


int ini_sget(
	ini_t *ini, const char *section, const char *key,
	const char *scanfmt, void *dst
) {
	const char *val = ini_get(ini, section, key);
	if (!val) {
	return 0;
	}
	if (scanfmt) {
	sscanf(val, scanfmt, dst);
	} else {
	*((const char**) dst) = val;
	}
	return 1;
}

