/*
	D4Player
	* clut.c
*/

#include "clut.h"
#include "../common.h"

#include <stdio.h>
#include <stdint.h>

int read_clut_header(clut_t *clut, FILE *fp, short encoding) {
	if (fread(&clut->header, CLUT_HEADER_SIZE, 1, fp) < 1) return 1;
	CONVERT_32(encoding, clut->header.length);
	return 0;
}

int read_clut_entries(clut_t *clut, FILE *fp) {
	pixel24_t *palette = clut->palette.palette;
	uint16_t r, g, b;
	int i, size = clut->palette.size;
	for (i=0; i<size; i++) {
		if (
			fread(&r, 2, 1, fp) < 1 ||
			fread(&g, 2, 1, fp) < 1 ||
			fread(&b, 2, 1, fp) < 1 ) return 1;
		CONVERT_16(BE_ENCODING, r);
		CONVERT_16(BE_ENCODING, g);
		CONVERT_16(BE_ENCODING, b);
		palette[i].r = (r>>8) & 0xFF;
		palette[i].g = (g>>8) & 0xFF;
		palette[i].b = (b>>8) & 0xFF;
	}
	for(; i<PAL_SIZE; i++)
		palette[i] = (pixel24_t){0,0,0};
	return 0;
}

int read_clut(clut_t *clut, FILE *fp, short encoding) {
	if (read_clut_header(clut, fp, encoding))
		return 1;
	
	clut->palette.size = clut->header.length/6;
	if (clut->palette.size > PAL_SIZE) {
		warning_f("Palette size is bigger than %d: %d", PAL_SIZE, clut->palette.size);
		clut->palette.size = PAL_SIZE;
	}
	
	if (read_clut_entries(clut, fp))
		return 1;
	
	return 0;
}

void print_clut(clut_t *clut) {
	printf(
		"-- CLUT\n"
		" header: %c%c%c%c\n"
		" length: %d\n"
		" palette size: %d colors\n"
		"\n",
		clut->header.header[0],
		clut->header.header[1],
		clut->header.header[2],
		clut->header.header[3],
		clut->header.length,
		clut->palette.size
	);
}
