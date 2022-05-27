#include "../include/GLFW/glfw3.h"

void SetWindowIconFromAscii(GLFWwindow* window) {
	const unsigned char iconPalette[3][4] = {
		{   0,   0,   0,   0 }, // Transparent
		{ 131, 118, 156, 255 }, // Purple
		{ 255, 241, 232, 255 }  // Peach
	};

	const char *const asciiLogo[] = {
		" pppppppppppppp ",
		"pp.....pp.....pp",
		"p..............p",
		"p..............p",
		"p..............p",
		"p..pppp..pppp..p",
		"p....pp..pp....p",
		"p..............p",
		"p..............p",
		"p..............p",
		"p.....pppp.....p",
		"p..............p",
		"p..............p",
		"pp............pp",
		"pp............pp",
		" pppppppppppppp "
	};

	int x, y;
	int height = 16;
	int width = 16;
	unsigned char *pixels = (unsigned char *)malloc(width * height * 4 * sizeof(unsigned char));
	unsigned char *pixelPtr = pixels;

	for (y = 0;  y < width;  y++) {
		for (x = 0;  x < height;  x++) {
			if (asciiLogo[y][x] == '.') { // if "." means peach
				memcpy(pixelPtr, iconPalette[2], 4);
			} else if (asciiLogo[y][x] == 'p') { // if "p" means purple
				memcpy(pixelPtr, iconPalette[1], 4);
			} else { // else transparent
				memcpy(pixelPtr, iconPalette[0], 4);
			}
			pixelPtr += 4;
		}
	}

	GLFWimage programIcon[1];
	programIcon[0].width = 16;
	programIcon[0].height = 16;
	programIcon[0].pixels = pixels;
	glfwSetWindowIcon(window, 1, programIcon);
}
