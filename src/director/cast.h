/*
	D4Player
	* cast.h
	
	== CASt ==
	The CASt block contains information about the cast member.
	The contents vary based on the type of the cast.
	
	header:
	* 4 bytes char - name of the block (CASt)
	* 32 bit integer - length of the block
	* 16 bit integer - header size
	* 32 bit integer - adicional info block size
	
	-- Header starts here ( [length of the block] = 2 + 4 + [header size] + [adicional info block size] )
	* 8 bit integer - type of cast
	* 8 bit integer - flags // may be ommited
	
	Type of cast can be:
	* 1 - image, associated with BITD and THUM blocks
	* 4 - palette, associated with CLUT block (empty cast)
	* 6 - sound, associated with snd block  (empty cast)
	* ? - still unknown
	Macromedia Director is known to support video, buttons and other types of cast.
	
	After the header and type specific data, comes an additional block of data.
	It's contents vary. In some casts, it contains the filename stored as Pascal string.
	
*/

#ifndef _CAST_H
#define _CAST_H

#include "rect.h"

#include <stdio.h>
#include <stdint.h>

#define CAST_TYPE_IMAGE     1
#define CAST_TYPE_FILMLOOP  2
#define CAST_TYPE_TEXT      3
#define CAST_TYPE_PALETTE   4
#define CAST_TYPE_PICTURE   5
#define CAST_TYPE_SOUND     6
#define CAST_TYPE_BUTTON    7
#define CAST_TYPE_SHAPE     8
#define CAST_TYPE_MOVIE     9
#define CAST_TYPE_VIDEO    10
#define CAST_TYPE_SCRIPT   11

#define CAST_SHAPE_RECTANGLE 1
#define CAST_SHAPE_ELLIPSE   3

typedef struct {
	int32_t header;
	int32_t length;
	int16_t header_size;
	int32_t additional_size;
	int8_t type;
} cast_header_t;

typedef struct {
	int8_t flags;
	int16_t unknown01;
	rect_t bounds;
	rect_t bounds2;
	int16_t center_x;
	int16_t center_y;
	int16_t bitdepth;
	int16_t unknown02;
} cast_image_t;

typedef struct {
	uint8_t flags;
	uint8_t unknown01;
	uint8_t type;
	rect_t bounds;
	uint16_t pattern;
	uint8_t fgcolor;
	uint8_t bgcolor;
	uint8_t filled;
	uint8_t border;
	uint8_t direction;
} cast_shape_t;

typedef struct {
	uint8_t flags;
	uint16_t unknown01;
	long offset;
	long size;
} cast_script_t;

typedef struct {
	cast_header_t header;
	cast_image_t image;
	cast_shape_t shape;
	cast_script_t script;
	char name[256];
} cast_t;

int read_cast(cast_t *cast, FILE *fp);
void print_cast(cast_t *cast);

#endif
