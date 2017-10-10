/*
	D4Player
	* clut.h
	
	== CLUT ==
	Color look-up table.
	This block contains the palette for color indexed images.
	It consists of [length/6] entries. Each entry is a RGB color stored as 16-bit integer triplets.
	
	Entry structure:
	* 16-bit integer - red value
	* 16-bit integer - green value
	* 16-bit integer - blue value
	
	The values stored on 16-bit integers have the same value for the high and low bytes:
	 - 0xFFFF, 0x5D5D, 0xA0A0, etc.
	Here, they are downsampled to 24-bit True color using only the high byte.
	
*/

#ifndef _CLUT_H
#define _CLUT_H

#include "palette.h"

#include <stdio.h>
#include <stdint.h>

typedef struct {
	char header[4];
	int32_t length;
} clut_header_t;

#define CLUT_HEADER_SIZE 8

#ifndef PAL_SIZE
#define PAL_SIZE 256
#endif

typedef struct {
	clut_header_t header;
	palette_t palette;
} clut_t;

int read_clut(clut_t *clut, FILE *fp, short encoding);
void print_clut(clut_t *clut);

#endif
