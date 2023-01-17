#include <glad/glad.h>

#include "./renderer.h"
#include "../logger.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

SDL_GLContext glContext;
SDL_Window* window = NULL;

int R_Init(SDL_Window* _window, bool vsyncEnabled) {
	window = _window;
	glContext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, glContext);

	// 0 for immediate updates, 1 for updates synchronized with the vertical retrace, -1 for adaptive vsync
	if (SDL_GL_SetSwapInterval(vsyncEnabled == true ? 1 : 0) != 0) {
		Logger_Error("Failed To Set Swap Interval: %s\n", SDL_GetError());
	}

	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		Logger_Error("Failed to initialize GLAD");
		return EXIT_FAILURE;
	}

	glEnable(GL_ALPHA_TEST);        
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");

	return EXIT_SUCCESS;
}

void R_Destroy(void) {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(glContext);
}

void R_Clear(void) {
	int w, h;
	SDL_GL_GetDrawableSize(window, &w, &h);
	glViewport(0, 0, w, h);
	glClearColor(0.075, 0.075, 0.1, 1.0); // Set The Color Used When Clearing Buffer (Set Alpha To 0 When Saving The Image So That The Color Doesn't Add In Final Render)
	glClear(GL_COLOR_BUFFER_BIT); // Clear The Back Buffer With The Color Specified Above
}

void R_NewFrame(void) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void R_Present(void) {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(window); // Swap Front & Back Buffers
}
