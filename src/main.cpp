#ifdef _WIN32
	#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include <chrono>
#include <thread>

#include "../include/imgui/imgui.h"
#include "../include/imgui/imgui_impl_glfw.h"
#include "../include/imgui/imgui_impl_opengl3.h"
#include "../include/tinyfiledialogs.h"

#include "../include/glad/glad.h"
#include "../include/GLFW/glfw3.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../lib/stb/stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb/stb_image.h"

#include "shader.h"
#include "math_linear.h"
#include "main.h"

std::string FILE_NAME = "untitled.png"; // Default Output Filename
char const * fileFilterPatterns[1] = { "*.png" };
unsigned char NumOfFilterPatterns = 1;

int WINDOW_DIMS[2] = {700, 500}; // Default Window Dimensions
int DIMS[2] = {60, 40}; // Width, Height Default Canvas Size

unsigned char *canvas_data; // Canvas Data Containg Pixel Values.

unsigned char last_palette_index = 1;
unsigned char palette_index = 1;
unsigned char palette_count = 17;
unsigned char palette[17][4] = {
	{ 0,   0,   0,   0   }, // Black Transparent/None
	// Pico 8 Color Palette - https://lospec.com/palette-list/pico-8
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

// NO_MODE defines that there shouldn't be anything drawn
enum mode { SQUARE_BRUSH, CIRCLE_BRUSH, PAN, FILL };
unsigned char Canvas_Freeze = 0;

unsigned char zoom_index = 3; // Default Zoom Level - 16
unsigned char zoom[8] = { 1, 2, 4, 8, 16, 32, 64, 128 }; // Zoom Levels
std::string zoomText = "Zoom: " + std::to_string(zoom[zoom_index]) + "x"; // Human Readable string decribing zoom level for UI
unsigned char brush_size = 5; // Default Brush Size

// Holds if a ctrl/shift is pressed or not
unsigned char ctrl = 0;
unsigned char shift = 0;

enum mode mode = CIRCLE_BRUSH;
enum mode last_mode = CIRCLE_BRUSH;
unsigned char *draw_colour; // Holds Pointer To Currently Selected Color
unsigned char erase[4] = { 0, 0, 0, 0 }; // Erase Color, Transparent Black.
unsigned char should_save = 0;

GLfloat viewport[4];
GLfloat canvasVertices[] = {
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
unsigned int indices[] = {0, 1, 3, 1, 2, 3};

double cursor_pos[2];
double cursor_pos_last[2];
double cursor_pos_relative[2];

int main(int argc, char **argv) {
	for (unsigned char i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-f") == 0) {
			FILE_NAME = argv[i+1];
			load_image_to_canvas();
			i++;
		}

		if (strcmp(argv[i], "-d") == 0) {
			int w, h;
			string_to_int(&w, argv[i + 1]);
			string_to_int(&h, argv[i + 2]);
			DIMS[0] = w;
			DIMS[1] = h;
			i += 2;
		}

		if (strcmp(argv[i], "-o") == 0) {
			FILE_NAME = argv[i + 1];
			i++;
		}

		if (strcmp(argv[i], "-w") == 0) {
			int w, h;
			string_to_int(&w, argv[i + 1]);
			string_to_int(&h, argv[i + 2]);
			WINDOW_DIMS[0] = w;
			WINDOW_DIMS[1] = h;
			i += 2;
		}

		if (strcmp(argv[i], "-p") == 0) {
			palette_count = 0;
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
					printf("Invalid colour in palette, check the length is 6 or 8.\n");
					break;
				}

				r = number >> start & 0xff;
				g = number >> (start - 8) & 0xff;
				b = number >> (start - 16) & 0xff;

				palette[palette_count + 1][0] = r;
				palette[palette_count + 1][1] = g;
				palette[palette_count + 1][2] = b;
				palette[palette_count + 1][3] = a;

				printf("Adding colour: #%s - rgb(%d, %d, %d)\n", argv[i], r, g, b);

				palette_count++;
				i++;
			}
		}
	}

	if (canvas_data == NULL) {
		canvas_data = (unsigned char *)malloc(DIMS[0] * DIMS[1] * 4 * sizeof(unsigned char));
		memset(canvas_data, 0, DIMS[0] * DIMS[1] * 4 * sizeof(unsigned char));
		if (canvas_data == NULL) {
			printf("Unable To allocate memory for canvas.\n");
			return 1;
		}
	}

	GLFWwindow *window;
	GLFWcursor *cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);

	draw_colour = palette[palette_index];

	glfwInit();
	glfwSetErrorCallback(logGLFWErrors);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_CENTER_CURSOR, GLFW_TRUE);

	window = glfwCreateWindow(WINDOW_DIMS[0], WINDOW_DIMS[1], "CSprite", NULL, NULL);

	if (!window) {
		printf("Failed to create GLFW window\n");
		free(canvas_data);
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowTitle(window, ("CSprite - " + FILE_NAME.substr(FILE_NAME.find_last_of("/\\") + 1)).c_str());
	glfwSwapInterval(0);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed to init GLAD\n");
		free(canvas_data);
		return 1;
	}

	glfwSetCursor(window, cursor);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Initial Canvas Position
	viewport[0] = (float)WINDOW_DIMS[0] / 2 - (float)DIMS[0] * zoom[zoom_index] / 2; // X Position
	viewport[1] = (float)WINDOW_DIMS[1] / 2 - (float)DIMS[1] * zoom[zoom_index] / 2; // Y Position

	// Output Width And Height Of The Canvas
	viewport[2] = DIMS[0] * zoom[zoom_index]; // Width
	viewport[3] = DIMS[1] * zoom[zoom_index]; // Height

	viewport_set();
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// If not a release build use the local shader files to edit shaders without problem
#ifndef NDEBUG
	unsigned int shader_program = create_shader_program("shader.vs", "shader.fs", NULL);
#else
	unsigned int shader_program = create_shader_program(NULL, NULL, NULL);
#endif

	unsigned int vertexBuffObj, vertexArrObj, ebo;
	glGenVertexArrays(1, &vertexArrObj);
	glGenBuffers(1, &vertexBuffObj);
	glGenBuffers(1, &ebo);
	glBindVertexArray(vertexArrObj);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(canvasVertices), canvasVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DIMS[0], DIMS[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas_data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL; // Disable Generation of .ini file

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoMove;

#ifndef NDEBUG
	double lastTime = glfwGetTime();
	int nbFrames = 0; // Number Of Frames Rendered
#endif

	auto const wait_time = std::chrono::milliseconds{ 17 };
	auto const start_time = std::chrono::steady_clock::now();
	auto next_time = start_time + wait_time;

	unsigned char showNewCanvasWindow = 0; // Holds Whether to show new canvas window or not.
	int NEW_DIMS[2] = {60, 40}; // Default Width, Height New Canvas if Created One

	while (!glfwWindowShouldClose(window)) {
#ifndef NDEBUG
		double currentTime = glfwGetTime(); // Uncomment This Block And Above 2 Commented Lines To Get Frame Time (Updated Every 1 Second)
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}
#endif

		std::this_thread::sleep_until(next_time);

		glfwPollEvents();
		process_input(window);

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

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DIMS[0], DIMS[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas_data);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New")) {
				showNewCanvasWindow = 1;
			}
			if (ImGui::MenuItem("Open")) {
				char *filePath = tinyfd_openFileDialog("Open A File", NULL, NumOfFilterPatterns, fileFilterPatterns, "Image File (.png)", 0);
				if (filePath != NULL) {
					FILE_NAME = std::string(filePath);
					load_image_to_canvas();
					glfwSetWindowTitle(window, ("CSprite - " + FILE_NAME.substr(FILE_NAME.find_last_of("/\\") + 1)).c_str()); // Simple Hack To Get The File Name from the path and set it to the window title
				}
			}
			if (ImGui::MenuItem("Save")) {
				save_image_from_canvas();
			}
			if (ImGui::MenuItem("Save As")) {
				char *filePath = tinyfd_saveFileDialog("Save A File", NULL, NumOfFilterPatterns, fileFilterPatterns, "Image File (.png)");
				if (filePath != NULL) {
					FILE_NAME = std::string(filePath);
					save_image_from_canvas();
					glfwSetWindowTitle(window, ("CSprite - " + FILE_NAME.substr(FILE_NAME.find_last_of("/\\") + 1)).c_str()); // Simple Hack To Get The File Name from the path and set it to the window title
				}
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();

		if (showNewCanvasWindow == 1) {
			Canvas_Freeze = 1;
			ImGui::SetNextWindowSize({280, 100}, 0);
			ImGui::Begin("NewCanvasWindow", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

			ImGui::InputInt("width", &NEW_DIMS[0], 1, 1, 0);
			ImGui::InputInt("height", &NEW_DIMS[1], 1, 1, 0);

			if (ImGui::Button("Ok")) {
				free(canvas_data);
				DIMS[0] = NEW_DIMS[0];
				DIMS[1] = NEW_DIMS[1];
				canvas_data = (unsigned char *)malloc(DIMS[0] * DIMS[1] * 4 * sizeof(unsigned char));
				memset(canvas_data, 0, DIMS[0] * DIMS[1] * 4 * sizeof(unsigned char));
				if (canvas_data == NULL) {
					printf("Unable To allocate memory for canvas.\n");
					return 1;
				}

				// Update Viewport
				viewport[0] = (float)WINDOW_DIMS[0] / 2 - (float)DIMS[0] * zoom[zoom_index] / 2;
				viewport[1] = (float)WINDOW_DIMS[1] / 2 - (float)DIMS[1] * zoom[zoom_index] / 2;
				viewport[2] = DIMS[0] * zoom[zoom_index];
				viewport[3] = DIMS[1] * zoom[zoom_index];
				viewport_set();

				Canvas_Freeze = 0;
				showNewCanvasWindow = 0;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				Canvas_Freeze = 0;
				showNewCanvasWindow = 0;
			}

			ImGui::End();
		}

		ImGui::Begin("ToolAndZoomWindow", NULL, window_flags);
		ImGui::SetWindowPos({0, 20});
		ImGui::SetWindowSize({(float)WINDOW_DIMS[0]/2, (float)WINDOW_DIMS[1]}); // Make Sure Text is visible everytime.

		switch (mode) {
			case SQUARE_BRUSH:
				if (palette_index == 0)
					ImGui::Text("Square Eraser - (Size: %d)", brush_size);
				else
					ImGui::Text("Square Brush - (Size: %d)", brush_size);
				break;
			case CIRCLE_BRUSH:
				if (palette_index == 0) {
					ImGui::Text("Circle Eraser - (Size: %d)", brush_size);
				} else {
					ImGui::Text("Circle Brush - (Size: %d)", brush_size);
				}
				break;
			case FILL:
				ImGui::Text("Fill");
				break;
			case PAN:
				ImGui::Text("Panning");
				break;
		}

		ImGui::Text("%s", zoomText.c_str());
		ImGui::End();

		ImGui::Begin("ColorPaletteWindow", NULL, window_flags);
		ImGui::SetWindowPos({0, (float)WINDOW_DIMS[1] - 35});
		for (int i = 1; i < palette_count; i++) {
			if (i != 1) ImGui::SameLine();
			if (ImGui::ColorButton(palette_index == i ? "Selected Color" : "Color", {(float)palette[i][0]/255, (float)palette[i][1]/255, (float)palette[i][2]/255, (float)palette[i][3]/255})) {
				palette_index = i;
			}
		};
		ImGui::End();

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
	return 0;
}

unsigned char * get_char_data(unsigned char *data, int x, int y) {
	return data + ((y * DIMS[0] + x) * 4);
}

void logGLFWErrors(int error, const char *description) {
	std::cout << description << std::endl;
}

void framebuffer_size_callback(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	WINDOW_DIMS[0] = width;
	WINDOW_DIMS[1] = height;

	// Center The Canvas On X, Y
	viewport[0] = (float)WINDOW_DIMS[0] / 2 - (float)DIMS[0] * zoom[zoom_index] / 2;
	viewport[1] = (float)WINDOW_DIMS[1] / 2 - (float)DIMS[1] * zoom[zoom_index] / 2;

	// Set The Canvas Size (Not Neccessary Here Tho)
	viewport[2] = DIMS[0] * zoom[zoom_index];
	viewport[3] = DIMS[1] * zoom[zoom_index];
	viewport_set();
}

void process_input(GLFWwindow *window) {
	if (Canvas_Freeze == 1) return;

	int x, y;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		x = (int)(cursor_pos_relative[0] / zoom[zoom_index]);
		y = (int)(cursor_pos_relative[1] / zoom[zoom_index]);

		if (x >= 0 && x < DIMS[0] && y >= 0 && y < DIMS[1]) {
			switch (mode) {
				case SQUARE_BRUSH:
				case CIRCLE_BRUSH:
					draw(x, y);
					break;
				case FILL: {
					unsigned char *ptr = get_pixel(y, y);
					// Color Clicked On.
					unsigned char colour[4] = {
						*(ptr + 0),
						*(ptr + 1),
						*(ptr + 2),
						*(ptr + 3)
					};
					fill(x, y, colour);
					break;
				}
				default:
					break;
			}
		}
	}
}

void mouse_callback(GLFWwindow *window, double x, double y) {
	/* infitesimally small chance aside from startup */
	if (cursor_pos_last[0] != 0 && cursor_pos_last[1] != 0) {
		if (mode == PAN) {
			float xmov = (cursor_pos_last[0] - cursor_pos[0]);
			float ymov = (cursor_pos_last[1] - cursor_pos[1]);
			viewport[0] -= xmov;
			viewport[1] += ymov;
			viewport_set();
		}
	}
	cursor_pos_last[0] = cursor_pos[0];
	cursor_pos_last[1] = cursor_pos[1];
	cursor_pos[0] = x;
	cursor_pos[1] = y;
	cursor_pos_relative[0] = x - viewport[0];
	cursor_pos_relative[1] = (y + viewport[1]) - (WINDOW_DIMS[1] - viewport[3]);
}

void mouse_button_callback(GLFWwindow *window, int button, int down, int c) {
	// Will Use For SOMETHING in future.
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
	if (yoffset > 0)
		adjust_zoom(true);
	else
		adjust_zoom(false);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_RELEASE) {
		if (mods == GLFW_MOD_CONTROL)
			ctrl = 0;

		if (mods == GLFW_MOD_SHIFT)
			shift = 0;

		if (key == GLFW_KEY_I) {
			if (brush_size < 255) {
				brush_size++;
			}
		} else if (key == GLFW_KEY_O) {
			if (brush_size != 1) {
				brush_size--;
			}
		}
		if (key == GLFW_KEY_SPACE) {
			mode = last_mode;
		}
	}

	// if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	// 	glfwSetWindowShouldClose(window, GLFW_TRUE);

	// Color Changing
	if (action == GLFW_PRESS) {
		if (mods == GLFW_MOD_CONTROL) {
			ctrl = 1;

			// if ctrl key is pressed and + or - is pressed, adjust the zoom size
			if (key == GLFW_KEY_EQUAL) {
				adjust_zoom(true);
			} else if (key == GLFW_KEY_MINUS) {
				adjust_zoom(false);
			}
		}

		if (mods == GLFW_MOD_SHIFT) {
			shift = 1;
		}

		switch (key) {
			case GLFW_KEY_K:
				if (palette_index > 1) {
					palette_index--;
				}
				break;
			case GLFW_KEY_L:
				if (palette_index < palette_count-1) {
					palette_index++;
				}
				break;
			case GLFW_KEY_1:
				if (palette_count >= 1) {
					palette_index = shift ? 9 : 1;
				}
				break;
			case GLFW_KEY_2:
				if (palette_count >= 2) {
					palette_index = shift ? 10 : 2;
				}
				break;
			case GLFW_KEY_3:
				if (palette_count >= 3) {
					palette_index = shift ? 11 : 3;
				}
				break;
			case GLFW_KEY_4:
				if (palette_count >= 4) {
					palette_index = shift ? 12 : 4;
				}
				break;
			case GLFW_KEY_5:
				if (palette_count >= 5) {
					palette_index = shift ? 13 : 5;
				}
				break;
			case GLFW_KEY_6:
				if (palette_count >= 6) {
					palette_index = shift ? 14 : 6;
				}
				break;
			case GLFW_KEY_7:
				if (palette_count >= 7) {
					palette_index = shift ? 15 : 7;
				}
				break;
			case GLFW_KEY_8:
				if (palette_count >= 8) {
					palette_index = shift ? 16 : 8;
				}
				break;
			case GLFW_KEY_F:
				mode = FILL;
				break;
			case GLFW_KEY_B:
				mode = shift ? CIRCLE_BRUSH : SQUARE_BRUSH;
				palette_index = last_palette_index;
				break;
			case GLFW_KEY_E:
				mode = shift ? CIRCLE_BRUSH : SQUARE_BRUSH;
				if (palette_index != 0) {
					last_palette_index = palette_index;
					palette_index = 0;
				}
				break;
			case GLFW_KEY_SPACE:
				last_mode = mode;
				mode = PAN;
			case GLFW_KEY_S:
				if (mods == GLFW_MOD_ALT) { // Show Prompt To Save if Alt + S pressed
					char *filePath = tinyfd_saveFileDialog("Save A File", NULL, NumOfFilterPatterns, fileFilterPatterns, "Image File (.png)");
					if (filePath != NULL) {
						FILE_NAME = std::string(filePath);
						save_image_from_canvas();
						glfwSetWindowTitle(window, ("CSprite - " + FILE_NAME.substr(FILE_NAME.find_last_of("/\\") + 1)).c_str()); // Simple Hack To Get The File Name from the path and set it to the window title
					}
				} else if (ctrl == 1) { // Directly Save Don't Prompt
					save_image_from_canvas();
				}
				break;
			case GLFW_KEY_O: {
				if (ctrl == 1) {
					char *filePath = tinyfd_openFileDialog("Open A File", NULL, NumOfFilterPatterns, fileFilterPatterns, "Image File (.png)", 0);
					if (filePath != NULL) {
						FILE_NAME = std::string(filePath);
						load_image_to_canvas();
						glfwSetWindowTitle(window, ("CSprite - " + FILE_NAME.substr(FILE_NAME.find_last_of("/\\") + 1)).c_str()); // Simple Hack To Get The File Name from the path and set it to the window title
					}
				}
			}
			default:
				break;
		}
	}

	draw_colour = palette[palette_index];
}

void viewport_set() {
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void adjust_zoom(bool increase) {
	if (increase == true) {
		if (zoom_index < sizeof(zoom)-1) {
			zoom_index++;
		}
	} else {
		if (zoom_index > 0) {
			zoom_index--;
		}
	}

	viewport[0] = (float)WINDOW_DIMS[0] / 2 - (float)DIMS[0] * zoom[zoom_index] / 2;
	viewport[1] = (float)WINDOW_DIMS[1] / 2 - (float)DIMS[1] * zoom[zoom_index] / 2;
	viewport[2] = DIMS[0] * zoom[zoom_index];
	viewport[3] = DIMS[1] * zoom[zoom_index];

	viewport_set();
	zoomText = "Zoom: " + std::to_string(zoom[zoom_index]) + "x";
}

int string_to_int(int *out, char *s) {
	char *end;
	if (s[0] == '\0')
		return -1;
	long l = strtol(s, &end, 10);
	if (l > INT_MAX)
		return -2;
	if (l < INT_MIN)
		return -3;
	if (*end != '\0')
		return -1;
	*out = l;
	return 0;
}

int color_equal(unsigned char *a, unsigned char *b) {
	if (*(a + 0) == *(b + 0) && *(a + 1) == *(b + 1) && *(a + 2) == *(b + 2) &&
		*(a + 3) == *(b + 3)) {
		return 1;
	}
	return 0;
}

unsigned char * get_pixel(int x, int y) {
	return canvas_data + ((y * DIMS[0] + x) * 4);
}

void draw(int x, int y) {
	for (int yr = -brush_size/2; yr < brush_size/2+1; yr++) {
		for (int xr = -brush_size/2; xr < brush_size/2+1; xr++) {
			if (x+xr < 0 || x+xr >= DIMS[0] || y+yr < 0 || y+yr > DIMS[1])
				continue;

			if (mode == CIRCLE_BRUSH && xr*xr + yr*yr > brush_size / 2 * brush_size / 2)
				continue;

			unsigned char *ptr = get_pixel(x+xr, y+yr);

			// Set Pixel Color
			*ptr = draw_colour[0]; // Red
			*(ptr + 1) = draw_colour[1]; // Green
			*(ptr + 2) = draw_colour[2]; // Blue
			*(ptr + 3) = draw_colour[3]; // Alpha
		}
	}
}

// Fill Tool, Fills The Whole Canvas Using Recursion
void fill(int x, int y, unsigned char *old_colour) {
	unsigned char *ptr = get_pixel(x, y);
	if (color_equal(ptr, old_colour)) {
		*ptr = draw_colour[0];
		*(ptr + 1) = draw_colour[1];
		*(ptr + 2) = draw_colour[2];
		*(ptr + 3) = draw_colour[3];

		if (x != 0 && !color_equal(get_pixel(x - 1, y), draw_colour))
			fill(x - 1, y, old_colour);
		if (x != DIMS[0] - 1 && !color_equal(get_pixel(x + 1, y), draw_colour))
			fill(x + 1, y, old_colour);
		if (y != DIMS[1] - 1 && !color_equal(get_pixel(x, y + 1), draw_colour))
			fill(x, y + 1, old_colour);
		if (y != 0 && !color_equal(get_pixel(x, y - 1), draw_colour))
			fill(x, y - 1, old_colour);
	}
}

void load_image_to_canvas() {
	int x, y, c;
	unsigned char *image_data = stbi_load(FILE_NAME.c_str(), &x, &y, &c, 0);
	if (image_data == NULL) {
		printf("Unable to load image %s\n", FILE_NAME.c_str());
	} else {
		DIMS[0] = x;
		DIMS[1] = y;
		canvas_data = (unsigned char *)malloc(DIMS[0] * DIMS[1] * 4 * sizeof(unsigned char));
		memset(canvas_data, 0, DIMS[0] * DIMS[1] * 4 * sizeof(unsigned char));
		int j, k;
		unsigned char *ptr;
		unsigned char *iptr;
		for (j = 0; j < y; j++) {
			for (k = 0; k < x; k++) {
				ptr = get_pixel(k, j);
				iptr = get_char_data(image_data, k, j);
				*(ptr+0) = *(iptr+0);
				*(ptr+1) = *(iptr+1);
				*(ptr+2) = *(iptr+2);
				*(ptr+3) = *(iptr+3);
			}
		}
		stbi_image_free(image_data);
	}
}

void save_image_from_canvas() {
	unsigned char *data = (unsigned char *) malloc(DIMS[0] * DIMS[1] * 4 * sizeof(unsigned char));

	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_write_png(FILE_NAME.c_str(), DIMS[0], DIMS[1], 4, data, 0);

	free(data);
	should_save = 0;
}
