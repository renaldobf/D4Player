/*
	D4Player
	* palette.h
*/ 

#ifndef _PALETTE_H
#define _PALETTE_H

#include <stdint.h>

#ifndef PAL_SIZE
#define PAL_SIZE 256
#endif

typedef struct {
	uint8_t r, g, b, a;
} pixel32_t;

typedef struct {
	uint8_t r, g, b;
} pixel24_t;

typedef struct {
	uint8_t index;
} pixel8_t;

typedef struct {
	int size;
	pixel24_t palette[PAL_SIZE];
} palette_t;

void load_palette_file(palette_t *palette, const char *filename);

#endif
