#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

// For Converting Strings To LowerCase in FixFileExtension function
#include <algorithm>
#include <cctype>

#include <chrono>
#include <thread>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "tinyfiledialogs.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "assets.h"
#include "shader.h"
#include "math_linear.h"
#include "main.h"
#include "save.h"
#include "helpers.h"

std::string FilePath = "untitled.png"; // Default Output Filename
char const * FileFilterPatterns[3] = { "*.png", "*.jpg", "*.jpeg" };
unsigned char NumOfFilterPatterns = 3;

int WindowDims[2] = {700, 500}; // Default Window Dimensions
int CanvasDims[2] = {60, 40}; // Width, Height Default Canvas Size

unsigned char *CanvasData; // Canvas Data Containg Pixel Values.
#define CANVAS_SIZE_B CanvasDims[0] * CanvasDims[1] * 4 * sizeof(unsigned char)

unsigned char LastPaletteIndex = 1;
unsigned char PaletteIndex = 1;
unsigned char PaletteCount = 17;
unsigned char ColorPalette[17][4] = {
	{ 0,   0,   0,   0   }, // Black Transparent/None
	// Pico 8 Color Palette - https://lospec.com/ColorPalette-list/pico-8
	{ 0,   0,   0,   255 }, // Black Color
	{ 29,  43,  83,  255 }, // Dark Violet
	{ 126, 37,  83,  255 }, // Dark Pink
	{ 0,   135, 81,  255 }, // Dark Green
	{ 171, 82,  54,  255 }, // Dark Orange
	{ 95,  87,  79,  255 }, // Dark Brown
	{ 194, 195, 199, 255 }, // Grey
	{ 255, 241, 232, 255 }, // Seashell
	{ 255, 0,   77,  255 }, // Redish Pink
	{ 255, 163, 0,   255 }, // Orange
	{ 255, 236, 39,  255 }, // Yellow
	{ 0,   228, 54,  255 }, // Green
	{ 41,  173, 255, 255 }, // Blue
	{ 131, 118, 156, 255 }, // Light Purple
	{ 255, 119, 168, 255 }, // Pink
	{ 255, 204, 170, 255 }  // Pale Orange
};

unsigned int ZoomLevel = 8; // Default Zoom Level
std::string ZoomText = "Zoom: " + std::to_string(ZoomLevel) + "x"; // Human Readable string decribing zoom level for UI
unsigned char BrushSize = 5; // Default Brush Size

// Holds if a ctrl/shift is pressed or not
unsigned char IsCtrlDown = 0;
unsigned char IsShiftDown = 0;

enum mode_e { SQUARE_BRUSH, CIRCLE_BRUSH, PAN, FILL, INK_DROPPER };
unsigned char CanvasFreeze = 0;

// Currently & last selected tool
enum mode_e Mode = CIRCLE_BRUSH;
enum mode_e LastMode = CIRCLE_BRUSH;

unsigned char *SelectedColor; // Holds Pointer To Currently Selected Color
unsigned char ShouldSave = 0;
unsigned char ShowNewCanvasWindow = 0; // Holds Whether to show new canvas window or not.

GLfloat ViewPort[4];
GLfloat CanvasVertices[] = {
	//       Canvas              Color To       Texture
	//     Coordinates          Blend With     Coordinates
	//  X      Y      Z      R     G     B      X     Y
	   1.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 0.0f, // Top Right
	   1.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // Bottom Right
	  -1.0f, -1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f, // Bottom Left
	  -1.0f,  1.0f,  0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 0.0f  // Top Left
	// Z Coordinates Are 0 Because We Are Working With 2D Stuff
	// Color To Blend With Are The Colors Which Will Be multiplied by the selected color to get the final output on the canvas
};

// Index Buffer
unsigned int Indices[] = {0, 1, 3, 1, 2, 3};

// Mouse Position On Window
double MousePos[2];
double MousePosLast[2];

// Mouse Position On Canvas
double MousePosRelative[2];
double MousePosRelativeLast[2];

bool DidUndo = false;
bool IsDirty = false;

struct cvstate {
	unsigned char* pixelData;
	cvstate* next; // Canvas State Before This Node
	cvstate* prev; // Canvas State After This Node
};

typedef struct cvstate cvstate_t; // Canvas State Type

cvstate_t* CurrentState = NULL;

int main(int argc, char **argv) {
	for (unsigned char i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0) {
			FilePath = argv[i+1];
			LoadImageToCanvas(FilePath.c_str(), CanvasDims, &CanvasData);
			i++;
		}

		if (strcmp(argv[i], "-d") == 0) {
			int w, h;
			string_to_int(&w, argv[i + 1]);
			string_to_int(&h, argv[i + 2]);
			CanvasDims[0] = w;
			CanvasDims[1] = h;
			i += 2;
		}

		if (strcmp(argv[i], "-o") == 0) {
			FilePath = argv[i + 1];
			i++;
		}

		if (strcmp(argv[i], "-w") == 0) {
			int w, h;
			string_to_int(&w, argv[i + 1]);
			string_to_int(&h, argv[i + 2]);
			WindowDims[0] = w;
			WindowDims[1] = h;
			i += 2;
		}

		if (strcmp(argv[i], "-p") == 0) {
			PaletteCount = 0;
			i++;

			while (i < argc && (strlen(argv[i]) == 6 || strlen(argv[i]) == 8)) {
				long number = (long)strtol(argv[i], NULL, 16);
				int start;
				unsigned char r, g, b, a;

				if (strlen(argv[i]) == 6) {
					start = 16;
					a = 255;
				} else if (strlen(argv[i]) == 8) {
					start = 24;
					a = number >> (start - 24) & 0xff;
				} else {
					printf("Invalid color in ColorPalette, check the length is 6 or 8.\n");
					break;
				}

				r = number >> start & 0xff;
				g = number >> (start - 8) & 0xff;
				b = number >> (start - 16) & 0xff;

				ColorPalette[PaletteCount + 1][0] = r;
				ColorPalette[PaletteCount + 1][1] = g;
				ColorPalette[PaletteCount + 1][2] = b;
				ColorPalette[PaletteCount + 1][3] = a;

				printf("Adding color: #%s - rgb(%d, %d, %d)\n", argv[i], r, g, b);

				PaletteCount++;
				i++;
			}
		}
	}

	if (CanvasData == NULL) {
		CanvasData = (unsigned char *)malloc(CANVAS_SIZE_B);
		memset(CanvasData, 0, CANVAS_SIZE_B);
		if (CanvasData == NULL) {
			printf("Unable To allocate memory for canvas.\n");
			return 1;
		}
	}

	GLFWwindow *window;
	GLFWcursor *cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);

	SelectedColor = ColorPalette[PaletteIndex];

	glfwInit();
	glfwSetErrorCallback(logGLFWErrors);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);

	window = glfwCreateWindow(WindowDims[0], WindowDims[1], "CSprite", NULL, NULL);

	if (!window) {
		printf("Failed to create GLFW window\n");
		free(CanvasData);
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowTitle(window, ("CSprite - " + FilePath.substr(FilePath.find_last_of("/\\") + 1)).c_str());
	glfwSwapInterval(0);

	// Conditionally Enable/Disable Window icon to reduce compile time in debug mode.
#ifdef ENABLE_WIN_ICON
	GLFWimage iconArr[3];
	iconArr[0].width = 16;
	iconArr[0].height = 16;
	iconArr[0].pixels = (unsigned char*)assets_get("data/icons/icon-16.png", NULL);

	iconArr[1].width = 32;
	iconArr[1].height = 32;
	iconArr[1].pixels = (unsigned char*)assets_get("data/icons/icon-32.png", NULL);

	iconArr[2].width = 48;
	iconArr[2].height = 48;
	iconArr[2].pixels = (unsigned char*)assets_get("data/icons/icon-48.png", NULL);
	glfwSetWindowIcon(window, 3, iconArr);
#endif

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to init GLAD\n");
		free(CanvasData);
		return 1;
	}

	glfwSetCursor(window, cursor);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initial Canvas Position
	ViewPort[0] = (float)WindowDims[0] / 2 - (float)CanvasDims[0] * ZoomLevel / 2; // X Position
	ViewPort[1] = (float)WindowDims[1] / 2 - (float)CanvasDims[1] * ZoomLevel / 2; // Y Position

	// Output Width And Height Of The Canvas
	ViewPort[2] = CanvasDims[0] * ZoomLevel; // Width
	ViewPort[3] = CanvasDims[1] * ZoomLevel; // Height

	ZoomNLevelViewport();
	glfwSetWindowSizeCallback(window, WindowSizeCallback);
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	unsigned int shader_program = CreateShaderProgram(
		"data/shaders/vertex.glsl",
		"data/shaders/fragment.glsl", NULL
	);

	unsigned int vertexBuffObj, vertexArrObj, ebo;
	glGenVertexArrays(1, &vertexArrObj);
	glGenBuffers(1, &vertexBuffObj);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vertexArrObj);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CanvasVertices), CanvasVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CanvasDims[0], CanvasDims[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, CanvasData);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL; // Disable Generation of .ini file


	const void* Montserrat_Bold = NULL;
	int Montserrat_Bold_Size = 0;

	Montserrat_Bold = assets_get("data/fonts/Montserrat-Bold.ttf", &Montserrat_Bold_Size);

	io.Fonts->AddFontFromMemoryCompressedTTF(Montserrat_Bold, Montserrat_Bold_Size, 16.0f);

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;

#ifdef SHOW_FRAME_TIME
	double lastTime = glfwGetTime();
	int nbFrames = 0; // Number Of Frames Rendered
#endif

	auto const wait_time = std::chrono::milliseconds{ 17 };
	auto const start_time = std::chrono::steady_clock::now();
	auto next_time = start_time + wait_time;

	int NEW_DIMS[2] = {60, 40}; // Default Width, Height New Canvas if Created One

	while (!glfwWindowShouldClose(window)) {
		// --------------------------------------------------------------------------------------
		// Updating Cursor Position Here because function callback was causing performance issues.
		glfwGetCursorPos(window, &MousePos[0], &MousePos[1]);
		/* infitesimally small chance aside from startup */
		if (MousePosLast[0] != 0 && MousePosLast[1] != 0) {
			if (Mode == PAN) {
				ViewPort[0] -= MousePosLast[0] - MousePos[0];
				ViewPort[1] += MousePosLast[1] - MousePos[1];
				ViewportSet();
			}
		}
		MousePosLast[0] = MousePos[0];
		MousePosLast[1] = MousePos[1];

		MousePosRelativeLast[0] = MousePosRelative[0];
		MousePosRelativeLast[1] = MousePosRelative[1];
		MousePosRelative[0] = MousePos[0] - ViewPort[0];
		MousePosRelative[1] = (MousePos[1] + ViewPort[1]) - (WindowDims[1] - ViewPort[3]);
		// --------------------------------------------------------------------------------------

#ifdef SHOW_FRAME_TIME
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) {
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}
#endif

		std::this_thread::sleep_until(next_time);

		glfwPollEvents();
		ProcessInput(window);

		glClearColor(0.075, 0.075, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);
		glBindVertexArray(vertexArrObj);
		glBindTexture(GL_TEXTURE_2D, 0);

		unsigned int alpha_loc = glGetUniformLocation(shader_program, "alpha");
		glUniform1f(alpha_loc, 0.2f);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glUniform1f(alpha_loc, 1.0f);
		glBindTexture(GL_TEXTURE_2D, texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CanvasDims[0], CanvasDims[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, CanvasData);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New", "Ctrl+N")) {
					ShowNewCanvasWindow = 1;
				}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					char *filePath = tinyfd_openFileDialog("Open A File", NULL, NumOfFilterPatterns, FileFilterPatterns, "Image File (.png, .jpg, .jpeg)", 0);
					if (filePath != NULL) {
						FilePath = std::string(filePath);
						LoadImageToCanvas(FilePath.c_str(), CanvasDims, &CanvasData);
						glfwSetWindowTitle(window, ("CSprite - " + FilePath.substr(FilePath.find_last_of("/\\") + 1)).c_str()); // Simple Hack To Get The File Name from the path and set it to the window title
						ZoomNLevelViewport();
					}
				}
				if (ImGui::BeginMenu("Save")) {
					if (ImGui::MenuItem("Save", "Ctrl+S")) {
						FilePath = FixFileExtension(FilePath);
						SaveImageFromCanvas(FilePath);
						glfwSetWindowTitle(window, ("CSprite - " + FilePath.substr(FilePath.find_last_of("/\\") + 1)).c_str()); // Simple Hack To Get The File Name from the path and set it to the window title
					}
					if (ImGui::MenuItem("Save As", "Alt+S")) {
						char *filePath = tinyfd_saveFileDialog("Save A File", NULL, NumOfFilterPatterns, FileFilterPatterns, "Image File (.png, .jpg, .jpeg)");
						if (filePath != NULL) {
							FilePath = FixFileExtension(std::string(filePath));
							SaveImageFromCanvas(FilePath);
							glfwSetWindowTitle(window, ("CSprite - " + FilePath.substr(FilePath.find_last_of("/\\") + 1)).c_str()); // Simple Hack To Get The File Name from the path and set it to the window title
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit")) {
				if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
					Undo();
				}
				if (ImGui::MenuItem("Redo", "Ctrl+Y")) {
					Redo();
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help")) {
				if (ImGui::MenuItem("About")) {
					openUrl("https://github.com/pegvin/CSprite/wiki/About-CSprite");
				}
				if (ImGui::MenuItem("GitHub")) {
					openUrl("https://github.com/pegvin/CSprite");
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (ShowNewCanvasWindow == 1) {
			CanvasFreeze = 1;
			ImGui::SetNextWindowSize({280, 100}, 0);
			if (ImGui::Begin("NewCanvasWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
				ImGui::InputInt("width", &NEW_DIMS[0], 1, 1, 0);
				ImGui::InputInt("height", &NEW_DIMS[1], 1, 1, 0);

				if (ImGui::Button("Ok")) {
					free(CanvasData);
					CanvasDims[0] = NEW_DIMS[0];
					CanvasDims[1] = NEW_DIMS[1];
					CanvasData = (unsigned char *)malloc(CANVAS_SIZE_B);
					memset(CanvasData, 0, CANVAS_SIZE_B);
					if (CanvasData == NULL) {
						printf("Unable To allocate memory for canvas.\n");
						return 1;
					}

					ZoomNLevelViewport();
					FreeHistory();
					CanvasFreeze = 0;
					ShowNewCanvasWindow = 0;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel")) {
					CanvasFreeze = 0;
					ShowNewCanvasWindow = 0;
				}

				ImGui::End();
			}
		}

		if (ImGui::Begin("ToolAndZoomWindow", NULL, window_flags | ImGuiWindowFlags_NoBringToFrontOnFocus |  ImGuiWindowFlags_NoFocusOnAppearing)) {
			ImGui::SetWindowPos({0, 20});
			std::string selectedToolText;

			switch (Mode) {
				case SQUARE_BRUSH:
					if (PaletteIndex == 0)
						selectedToolText = "Square Eraser - (Size: " + std::to_string(BrushSize) + ")";
					else
						selectedToolText = "Square Brush - (Size: " + std::to_string(BrushSize) + ")";
					break;
				case CIRCLE_BRUSH:
					if (PaletteIndex == 0) {
						selectedToolText = "Circle Eraser - (Size: " + std::to_string(BrushSize) + ")";
					} else {
						selectedToolText = "Circle Brush - (Size: " + std::to_string(BrushSize) + ")";
					}
					break;
				case FILL:
					selectedToolText = "Fill";
					break;
				case INK_DROPPER:
					selectedToolText = "Ink Dropper";
					break;
				case PAN:
					selectedToolText = "Panning";
					break;
			}

			ImVec2 textSize1 = ImGui::CalcTextSize(selectedToolText.c_str(), NULL, false, -2.0f);
			ImVec2 textSize2 = ImGui::CalcTextSize(ZoomText.c_str(), NULL, false, -2.0f);
			ImGui::SetWindowSize({(float)(textSize1.x + textSize2.x), (float)(textSize1.y + textSize2.y) * 2}); // Make Sure Text is visible everytime.

			ImGui::Text("%s", selectedToolText.c_str());
			ImGui::Text("%s", ZoomText.c_str());
			ImGui::End();
		}

		if (ImGui::Begin("ColorPaletteWindow", NULL, window_flags)) {
			ImGui::SetWindowPos({0, (float)WindowDims[1] - 35});
			for (int i = 1; i < PaletteCount; i++) {
				if (i != 1) ImGui::SameLine();
				if (ImGui::ColorButton(PaletteIndex == i ? "Selected Color" : ("Color##" + std::to_string(i)).c_str(), {(float)ColorPalette[i][0]/255, (float)ColorPalette[i][1]/255, (float)ColorPalette[i][2]/255, (float)ColorPalette[i][3]/255})) {
					PaletteIndex = i;
					SelectedColor = ColorPalette[PaletteIndex];
				}
			};
			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		next_time += wait_time;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
	FreeHistory();
	return 0;
}

unsigned char * GetCharData(unsigned char *data, int x, int y) {
	return data + ((y * CanvasDims[0] + x) * 4);
}

void FrameBufferSizeCallback(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
}

void ZoomNLevelViewport() {
	// Simple hacky way to adjust canvas zoom level till it fits the window
	while (true) {
		if (ViewPort[2] >= WindowDims[1] || ViewPort[3] >= WindowDims[1]) {
			AdjustZoom(false);
			AdjustZoom(false);
			AdjustZoom(false);
			break;
		}
		AdjustZoom(true);
		ViewPort[2] = CanvasDims[0] * ZoomLevel;
		ViewPort[3] = CanvasDims[1] * ZoomLevel;
	}

	// Center On Screen
	ViewPort[0] = (float)WindowDims[0] / 2 - (float)CanvasDims[0] * ZoomLevel / 2;
	ViewPort[1] = (float)WindowDims[1] / 2 - (float)CanvasDims[1] * ZoomLevel / 2;
	ViewportSet();
}

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
	WindowDims[0] = width;
	WindowDims[1] = height;

	// Center The Canvas On X, Y
	ViewPort[0] = (float)WindowDims[0] / 2 - (float)CanvasDims[0] * ZoomLevel / 2;
	ViewPort[1] = (float)WindowDims[1] / 2 - (float)CanvasDims[1] * ZoomLevel / 2;

	// Set The Canvas Size (Not Neccessary Here Tho)
	ViewPort[2] = CanvasDims[0] * ZoomLevel;
	ViewPort[3] = CanvasDims[1] * ZoomLevel;
	ViewportSet();
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		int x = (int)(MousePosRelative[0] / ZoomLevel);
		int y = (int)(MousePosRelative[1] / ZoomLevel);

		if (x >= 0 && x < CanvasDims[0] && y >= 0 && y < CanvasDims[1] && (Mode == SQUARE_BRUSH || Mode == CIRCLE_BRUSH || Mode == FILL)) {
			if (action == GLFW_PRESS) {
				SaveState();
			}
			if (action == GLFW_RELEASE) {
				if (DidUndo == true) {
					IsDirty = true;
					DidUndo = false;
				} else {
					IsDirty = false;
				}
				SaveState();
			}
		}
	}
}

void ProcessInput(GLFWwindow *window) {
	if (CanvasFreeze == 1) return;

	int x, y;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		x = (int)(MousePosRelative[0] / ZoomLevel);
		y = (int)(MousePosRelative[1] / ZoomLevel);

		if (x >= 0 && x < CanvasDims[0] && y >= 0 && y < CanvasDims[1]) {
			switch (Mode) {
				case SQUARE_BRUSH:
				case CIRCLE_BRUSH: {
					draw(x, y);
					drawInBetween(x, y, (int)(MousePosRelativeLast[0] / ZoomLevel), (int)(MousePosRelativeLast[1] / ZoomLevel));
					break;
				}
				case FILL: {
					unsigned char *ptr = GetPixel(x, y);
					// Color Clicked On.
					unsigned char color[4] = {
						*(ptr + 0),
						*(ptr + 1),
						*(ptr + 2),
						*(ptr + 3)
					};
					fill(x, y, color);
					break;
				}
				case INK_DROPPER: {
					unsigned char *ptr = GetPixel(x, y);
					// Color Clicked On.
					unsigned char color[4] = {
						*(ptr + 0),
						*(ptr + 1),
						*(ptr + 2),
						*(ptr + 3)
					};

					// For loop starts from 1 because we don't need the first color i.e. 0,0,0,0 or transparent black
					for (int i = 1; i < PaletteCount; i++) {
						if (color_equal(ColorPalette[i], color) == 1) {
							LastPaletteIndex = PaletteIndex;
							PaletteIndex = i;
							break;
						}
					}
					break;
				}
				default: {
					break;
				}
			}
		}
	}
}

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	if (yoffset > 0)
		AdjustZoom(true);
	else
		AdjustZoom(false);
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		if (mods == GLFW_MOD_CONTROL)
			IsCtrlDown = 0;

		if (mods == GLFW_MOD_SHIFT)
			IsShiftDown = 0;

		if (key == GLFW_KEY_SPACE) {
			Mode = LastMode;
		}
	}

	if (action == GLFW_PRESS) {
		if (mods == GLFW_MOD_CONTROL) {
			IsCtrlDown = 1;

			// if IsCtrlDown key is pressed and + or - is pressed, adjust the zoom size
			if (key == GLFW_KEY_EQUAL) {
				AdjustZoom(true);
			} else if (key == GLFW_KEY_MINUS) {
				AdjustZoom(false);
			}
		} else if (mods == GLFW_MOD_SHIFT) {
			IsShiftDown = 1;
		} else {
			if (key == GLFW_KEY_EQUAL) {
				if (BrushSize < 255) {
					BrushSize++;
				}
			} else if (key == GLFW_KEY_MINUS) {
				if (BrushSize != 1) {
					BrushSize--;
				}
			}
		}

		switch (key) {
			case GLFW_KEY_K:
				if (PaletteIndex > 1) {
					PaletteIndex--;
				}
				break;
			case GLFW_KEY_L:
				if (PaletteIndex < PaletteCount-1) {
					PaletteIndex++;
				}
				break;
			case GLFW_KEY_1:
				if (PaletteCount >= 1) {
					PaletteIndex = IsShiftDown ? 9 : 1;
				}
				break;
			case GLFW_KEY_2:
				if (PaletteCount >= 2) {
					PaletteIndex = IsShiftDown ? 10 : 2;
				}
				break;
			case GLFW_KEY_3:
				if (PaletteCount >= 3) {
					PaletteIndex = IsShiftDown ? 11 : 3;
				}
				break;
			case GLFW_KEY_4:
				if (PaletteCount >= 4) {
					PaletteIndex = IsShiftDown ? 12 : 4;
				}
				break;
			case GLFW_KEY_5:
				if (PaletteCount >= 5) {
					PaletteIndex = IsShiftDown ? 13 : 5;
				}
				break;
			case GLFW_KEY_6:
				if (PaletteCount >= 6) {
					PaletteIndex = IsShiftDown ? 14 : 6;
				}
				break;
			case GLFW_KEY_7:
				if (PaletteCount >= 7) {
					PaletteIndex = IsShiftDown ? 15 : 7;
				}
				break;
			case GLFW_KEY_8:
				if (PaletteCount >= 8) {
					PaletteIndex = IsShiftDown ? 16 : 8;
				}
				break;
			case GLFW_KEY_F:
				Mode = FILL;
				break;
			case GLFW_KEY_B:
				Mode = IsShiftDown ? SQUARE_BRUSH : CIRCLE_BRUSH;
				PaletteIndex = LastPaletteIndex;
				break;
			case GLFW_KEY_E:
				Mode = IsShiftDown ? SQUARE_BRUSH : CIRCLE_BRUSH;
				if (PaletteIndex != 0) {
					LastPaletteIndex = PaletteIndex;
					PaletteIndex = 0;
				}
				break;
			case GLFW_KEY_I:
				LastMode = Mode;
				Mode = INK_DROPPER;
				break;
			case GLFW_KEY_SPACE:
				LastMode = Mode;
				Mode = PAN;
				break;
			case GLFW_KEY_Z:
				if (IsCtrlDown == 1) {
					Undo();
				}
				break;
			case GLFW_KEY_Y:
				if (IsCtrlDown == 1) {
					Redo();
				}
				break;
			case GLFW_KEY_N:
				if (IsCtrlDown == 1) ShowNewCanvasWindow = 1;
				break;
			case GLFW_KEY_S:
				if (mods == GLFW_MOD_ALT) { // Show Prompt To Save if Alt + S pressed
					char *filePath = tinyfd_saveFileDialog("Save A File", NULL, NumOfFilterPatterns, FileFilterPatterns, "Image File (.png, .jpg, .jpeg)");
					if (filePath != NULL) {
						FilePath = FixFileExtension(std::string(filePath));
						SaveImageFromCanvas(FilePath);
						glfwSetWindowTitle(window, ("CSprite - " + FilePath.substr(FilePath.find_last_of("/\\") + 1)).c_str()); // Simple Hack To Get The File Name from the path and set it to the window title
					}
				} else if (IsCtrlDown == 1) { // Directly Save Don't Prompt
					FilePath = FixFileExtension(FilePath);
					SaveImageFromCanvas(FilePath);
				}
				break;
			case GLFW_KEY_O: {
				if (IsCtrlDown == 1) {
					char *filePath = tinyfd_openFileDialog("Open A File", NULL, NumOfFilterPatterns, FileFilterPatterns, "Image File (.png, .jpg, .jpeg)", 0);
					if (filePath != NULL) {
						FilePath = std::string(filePath);
						LoadImageToCanvas(FilePath.c_str(), CanvasDims, &CanvasData);
						glfwSetWindowTitle(window, ("CSprite - " + FilePath.substr(FilePath.find_last_of("/\\") + 1)).c_str()); // Simple Hack To Get The File Name from the path and set it to the window title
					}
				}
			}
			default:
				break;
		}
	}

	SelectedColor = ColorPalette[PaletteIndex];
}

void ViewportSet() {
	glViewport(ViewPort[0], ViewPort[1], ViewPort[2], ViewPort[3]);
}

void AdjustZoom(bool increase) {
	if (increase == true) {
		if (ZoomLevel < UINT_MAX) { // Max Value Of Unsigned int
			ZoomLevel++;
		}
	} else {
		if (ZoomLevel != 1) { // if zoom is 1 then don't decrease it further
			ZoomLevel--;
		}
	}

	// Comment Out To Not Center When Zooming
	ViewPort[0] = (float)WindowDims[0] / 2 - (float)CanvasDims[0] * ZoomLevel / 2;
	ViewPort[1] = (float)WindowDims[1] / 2 - (float)CanvasDims[1] * ZoomLevel / 2;

	ViewPort[2] = CanvasDims[0] * ZoomLevel;
	ViewPort[3] = CanvasDims[1] * ZoomLevel;

	ViewportSet();
	ZoomText = "Zoom: " + std::to_string(ZoomLevel) + "x";
}

unsigned char * GetPixel(int x, int y) {
	return CanvasData + ((y * CanvasDims[0] + x) * 4);
}

/*
	Function Takes 4 Argument First 2 Are starting x, y coordinates,
	and second 2 are ending x, y coordinates.
	And using a while loop it draws between the 2 given coordinates,
	hence no gap is left when mouse is being moved very fast
*/
void drawInBetween(int st_x, int st_y, int end_x, int end_y) {
	while (st_x != end_x || st_y != end_y) {
		if (st_x < end_x) {
			st_x++;
		}
		if (st_x > end_x) {
			st_x--;
		}
		if (st_y < end_y) {
			st_y++;
		}
		if (st_y > end_y) {
			st_y--;
		}

		for (int dirY = -BrushSize / 2; dirY < BrushSize / 2 + 1; dirY++) {
			for (int dirX = -BrushSize / 2; dirX < BrushSize / 2 + 1; dirX++) {
				if (st_x + dirX < 0 || st_x + dirX >= CanvasDims[0] || st_y + dirY < 0 || st_y + dirY > CanvasDims[1])
					continue;

				if (Mode == CIRCLE_BRUSH && dirX * dirX + dirY * dirY > BrushSize / 2 * BrushSize / 2)
					continue;

				unsigned char *ptr = GetPixel(st_x + dirX, st_y + dirY);

				// Set Pixel Color
				*ptr = SelectedColor[0]; // Red
				*(ptr + 1) = SelectedColor[1]; // Green
				*(ptr + 2) = SelectedColor[2]; // Blue
				*(ptr + 3) = SelectedColor[3]; // Alpha
			}
		}
	}
}

void draw(int st_x, int st_y) {
	// dirY = direction Y
	// dirX = direction X

	for (int dirY = -BrushSize / 2; dirY < BrushSize / 2 + 1; dirY++) {
		for (int dirX = -BrushSize / 2; dirX < BrushSize / 2 + 1; dirX++) {
			if (st_x + dirX < 0 || st_x + dirX >= CanvasDims[0] || st_y + dirY < 0 || st_y + dirY > CanvasDims[1])
				continue;

			if (Mode == CIRCLE_BRUSH && dirX * dirX + dirY * dirY > BrushSize / 2 * BrushSize / 2)
				continue;

			unsigned char *ptr = GetPixel(st_x + dirX, st_y + dirY);

			// Set Pixel Color
			*ptr = SelectedColor[0]; // Red
			*(ptr + 1) = SelectedColor[1]; // Green
			*(ptr + 2) = SelectedColor[2]; // Blue
			*(ptr + 3) = SelectedColor[3]; // Alpha
		}
	}
}

// Fill Tool, Fills The Whole Canvas Using Recursion
void fill(int x, int y, unsigned char *old_color) {
	unsigned char *ptr = GetPixel(x, y);
	if (color_equal(ptr, old_color)) {
		*ptr = SelectedColor[0];
		*(ptr + 1) = SelectedColor[1];
		*(ptr + 2) = SelectedColor[2];
		*(ptr + 3) = SelectedColor[3];

		if (x != 0 && !color_equal(GetPixel(x - 1, y), SelectedColor))
			fill(x - 1, y, old_color);
		if (x != CanvasDims[0] - 1 && !color_equal(GetPixel(x + 1, y), SelectedColor))
			fill(x + 1, y, old_color);
		if (y != CanvasDims[1] - 1 && !color_equal(GetPixel(x, y + 1), SelectedColor))
			fill(x, y + 1, old_color);
		if (y != 0 && !color_equal(GetPixel(x, y - 1), SelectedColor))
			fill(x, y - 1, old_color);
	}
}

// Makes sure that the file extension is .png or .jpg/.jpeg
std::string FixFileExtension(std::string filepath) {
	std::string fileExt = filepath.substr(filepath.find_last_of(".") + 1);
	std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), [](unsigned char c){ return std::tolower(c); });

	if (fileExt != "png" && fileExt != "jpg" && fileExt != "jpeg") {
		filepath = filepath + ".png";
	}

	return filepath;
}

void SaveImageFromCanvas(std::string filepath) {
	std::string fileExt = filepath.substr(filepath.find_last_of(".") + 1);
	// Convert File Extension to LowerCase
	std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(), [](unsigned char c){ return std::tolower(c); });

	if (fileExt == "png") {
		WritePngFromCanvas(filepath.c_str(), CanvasDims);
	} else if (fileExt == "jpg" || fileExt == "jpeg") {
		WriteJpgFromCanvas(filepath.c_str(), CanvasDims);
	} else {
		filepath = filepath + ".png";
		WritePngFromCanvas(filepath.c_str(), CanvasDims);
	}
	ShouldSave = 0;
}

/*
	Pushes Pixels On Current Canvas in "History" array at index "HistoryIndex"
	Removes The Elements in a range from "History" if "IsDirty" is true
*/
void SaveState() {
	if (IsDirty == true && CurrentState != NULL) {
		cvstate_t* tmp;
		cvstate_t* head = CurrentState->next; // we start freeing from the next node of current node

		while (head != NULL) {
			tmp = head;
			head = head->next;
			if (tmp->pixelData != NULL) {
				free(tmp->pixelData);
			}
			free(tmp);
		}
	}

	cvstate_t* NewState = (cvstate_t*) malloc(sizeof(cvstate_t));
	NewState->pixelData = (unsigned char*) malloc(CANVAS_SIZE_B);

	if (CurrentState == NULL) {
		CurrentState = NewState;
		CurrentState->prev = NULL;
		CurrentState->next = NULL;
	} else {
		NewState->prev = CurrentState;
		NewState->next = NULL;
		CurrentState->next = NewState;
		CurrentState = NewState;
	}

	memset(CurrentState->pixelData, 0, CANVAS_SIZE_B);
	memcpy(CurrentState->pixelData, CanvasData, CANVAS_SIZE_B);
}

// Undo - Puts The Pixels from "History" at "HistoryIndex"
int Undo() {
	DidUndo = true;

	if (CurrentState->prev != NULL) {
		CurrentState = CurrentState->prev;
		memcpy(CanvasData, CurrentState->pixelData, CANVAS_SIZE_B);
	}
	return 0;
}

// Redo - Puts The Pixels from "History" at "HistoryIndex"
int Redo() {
	if (CurrentState->next != NULL) {
		CurrentState = CurrentState->next;
		memcpy(CanvasData, CurrentState->pixelData, CANVAS_SIZE_B);
	}

	return 0;
}

/*
	Function: FreeHistory()
	Takes The CurrentState Node
		- Frees All Of The Nodes Before It
		- Frees All Of The Nodes After It
*/
void FreeHistory() {
	if (CurrentState == NULL) return;

	cvstate_t* tmp;
	cvstate_t* head = CurrentState->prev;

	while (head != NULL) {
		tmp = head;
		head = head->prev;
		if (tmp != NULL && tmp->pixelData != NULL) {
			free(tmp->pixelData);
			free(tmp);
		}
		tmp = NULL;
	}

	head = CurrentState;

	while (head != NULL) {
		tmp = head;
		head = head->next;
		if (tmp != NULL && tmp->pixelData != NULL) {
			free(tmp->pixelData);
			free(tmp);
		}
		tmp = NULL;
	}

	CurrentState = NULL;
}
