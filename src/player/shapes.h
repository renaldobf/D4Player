/*
	D4Player
	* shapes.h
*/

#ifndef _SHAPES_H
#define _SHAPES_H

#include "allegro.h"

#define SHAPE_RECTANGLE         1
#define SHAPE_ROUND_RECTANGLE   2
#define SHAPE_ELLIPSE           3
#define SHAPE_LINE_TL_BR        4
#define SHAPE_LINE_BL_TR        5

// Border radius for round rectangles
#define BORDER_RADIUS 12

void draw_shape(BITMAP *bmp, int left, int top, int type, int width, int height, int bgcolor, int fgcolor, int border, int pattern, short filled);
BITMAP *make_shape(int type, int width, int height, int bgcolor, int fgcolor, int border, int pattern, short filled);

#endif
