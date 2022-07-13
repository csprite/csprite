// Coverts a supported image file to an RGBA array
// To compile: gcc -lm -o image2inl image2inl.c

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void help(void) {
	fprintf(stderr, "Usage: image2inl <input_file> <symbolname> <output_file>\n");
	exit(1);
}

void convert(const char *ifname, const char *ofname, const char *symbolname) {
	// Check if input file exists
	FILE *ifile, *ofile;
	ifile = fopen(ifname, "rb");
	if (ifile == NULL) {
		fprintf(stderr, "cannot open %s for reading\n", ifname);
		exit(1);
	}
	fclose(ifile);

	// Check if output location is writable
	ofile = fopen(ofname, "wb");
	if (ofile == NULL) {
		fprintf(stderr, "cannot open %s for writing\n", ofname);
		exit(1);
	}

	// use STBI_rgb if you don't want/need the alpha channel or STBI_rgb_alpha
	int req_format = STBI_rgb_alpha;
	int width, height, orig_format;
	unsigned char* data = stbi_load(
		ifname, &width, &height, &orig_format, req_format
	);

	if (data == NULL) {
		printf("Loading image data failed: %s", stbi_failure_reason());
		exit(1);
	}

	int c, col = 1, inc = 0, size = (width * height) * 4;
	fprintf(ofile, "static unsigned int %s_len = %d;\n", symbolname, size);
	fprintf(ofile, "static unsigned char %s[] = {\n", symbolname);

	while (inc < size) {
		c = data[inc];
		if(col >= 78 - 6) {
			fputc('\n', ofile);
			col = 1;
		}
		if(col == 1) {
			fprintf(ofile, "  ");
		}
		fprintf(ofile, "0x%.2x, ", c);
		col += 6;
		inc++;
	}

	fprintf(ofile, "\n};\n");
	fclose(ofile);
}

int main(int argc, char **argv) {
	if(argc != 4) {
		help();
	}
	convert(argv[1], argv[3], argv[2]);
	return 0;
}
