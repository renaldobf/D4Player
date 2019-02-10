/*
	D4Player
	* score.h
	
	== VWSC ==
	VideoWorks Score
	It contains information about the score, i.e., the timing, sprites size and location, sound,
	function callings, palettes and transitions.
	
	This block is stored with Delta encoding.
	
	Header:
	* 4 byte char - block 4cc header
	* 32-bit integer - block size
	
	Score header
	* 32-bit integer - score block size
	* 32-bit integer - header size / padding to data
	* 32-bit integer - frame count
	* 16-bit integer - unknown
	* 16-bit integer - size of individual frames
	* 16-bit integer - channel count
	* 16-bit integer - unknown
	
	After the header, comes the delta encoded score.
	But before, let's take a look on how a score frame looks like.
	
	Each frame is composed of [channel count] entries. They all have the same size, [size of individual frames].
	* The first channel is the main channel. It has information about	the timing, sound, function callings and transitions.
	* The second is the palette channel.
	* The rest are the sprite channels. They use 1-based indexing for reference.
	
	Back to the encoding. Each frame is stored individually.
	The entries are as follow:
	
	* 16-bit integer - whole entry size
	If the size is 2, the actual frame is identical to the previous.
	
	* 16-bit integer - block size
	* 16-bit integer - offset
	Then comes [block size] bytes to be written to the offset [offset] on the frame.
	This is repeated untill [whole entry size] bytes are read.
	Then comes the next frame.
		
*/

#ifndef _SCORE_H
#define _SCORE_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
	int32_t header;
	int32_t length;
} vwsc_header_t;

typedef struct {
	int16_t flags;
	int16_t transition;
	int8_t transition_id;
	int8_t timing;
	int16_t sound1_cast;
	int16_t sound2_cast;
	int16_t sound_flags;
	int16_t unknown1;
	int16_t unknown2;
	int16_t script;
	int16_t unknown3;
} mainchannel_frame_t;

typedef struct {
	int16_t palette_cast;
	int16_t unknown2;
	int8_t transitio_fps;//
	int8_t unknown3;
	int16_t unknown4;
	int16_t palette_id;
	int16_t unknown5;
	int16_t unknown6;
	int16_t unknown7;
	int16_t unknown8;
	int16_t unknown9;
} palette_frame_t;

typedef struct {
	int16_t type;
	int16_t color;
	int16_t ink_type;
	int16_t cast;
	int16_t top;
	int16_t left;
	int16_t height;
	int16_t width;
	int16_t flag1;
	int16_t flag2;
} sprite_frame_t;

typedef struct {
	int32_t frame_count;
	int32_t channel_count;
	mainchannel_frame_t *mainchannel;
	palette_frame_t *palette;
	sprite_frame_t *frames;
} score_t;

#define VWSC_HEADER_SIZE 8
#define FRAME_SIZE 20

#define SCORE_4CC "VWSC"
#define SCORE_4CC_LE "CSWV"

#define INK_TYPE_COPY               0 // Copy
#define INK_TYPE_TRANSPARENT        1 // Transparent
#define INK_TYPE_REVERSE            2 // Reverse
#define INK_TYPE_GHOST              3 // Ghost
#define INK_TYPE_NOT_COPY           4 // Not Copy
#define INK_TYPE_NOT_TRANSPARENT    5 // Not Transparent
#define INK_TYPE_NOT_REVERSE        6 // Not Reverse
#define INK_TYPE_NOT_GHOST          7 // Not Ghost
#define INK_TYPE_MATTE              8 // Matte
#define INK_TYPE_MASK               9 // Mask
#define INK_TYPE_BLEND             32 // Blend
#define INK_TYPE_ADD_PIN           33 // Add Pin
#define INK_TYPE_ADD               34 // Add
#define INK_TYPE_SUB_PIN           35 // Subtract Pin
#define INK_TYPE_BKGND_TRANSPARENT 36 // Bkgnd Transparent
#define INK_TYPE_LIGHTEST          37 // Lightest
#define INK_TYPE_SUBTRACT          38 // Subtract
#define INK_TYPE_DARKEST           39 // Darkest

#define FRAME_TYPE_EMPTY            0 // Empty
#define FRAME_TYPE_IMAGE_CAST       1 // Image
#define FRAME_TYPE_RECTANGLE        2 // Rectangle
#define FRAME_TYPE_ROUND_RECTANGLE  3 // Round rectangle
#define FRAME_TYPE_ELLIPSE          4 // Ellipse
#define FRAME_TYPE_LINE_TL_BR       5 // Line from top-left to bottom-right
#define FRAME_TYPE_LINE_BL_TR       6 // Line from bottom-left to top-right
#define FRAME_TYPE_TEXT             7 // Text
#define FRAME_TYPE_BUTTON           8 // Button
#define FRAME_TYPE_CHECKBOX         9 // Checkbox
#define FRAME_TYPE_RADIO_BUTTON    10 // Radio button
#define FRAME_TYPE_SHAPE_CAST      16 // Unknown

#define is_opaque_shape(x) ((x)==INK_TYPE_COPY || (x)==INK_TYPE_MATTE)

#define FRAME_BORDER_WIDTH(x) (((x).ink_type >> 8) & 0x7)
#define FRAME_INK_TYPE(x) ((x).ink_type & 0x3f)

#define FRAME_BG_COLOR(x) (((uint16_t)(x).color) & 0xff)
#define FRAME_FG_COLOR(x) (((uint16_t)(x).color) >> 8)

int read_score(score_t *score, FILE *fp, short encoding);
void free_score(score_t *score);

#endif
