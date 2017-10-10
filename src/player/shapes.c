/*
	D4Player
	* shapes.c
*/

#include "shapes.h"
#include "../common.h"
#include "allegro.h"
#include "patterns.h"

void draw_shape(
		BITMAP *bmp, int left, int top, int type, int width, int height, int bgcolor, int fgcolor,
		int border, int pattern, short filled
) {
	int x1, x2, y1, y2;
	switch (type) {
		case SHAPE_RECTANGLE:
			x1 = left + border;
			y1 = top + border;
			x2 = left + width-1 - border;
			y2 = top + height-1 - border;
			if (filled) {
				if (pattern >= 1 && pattern <= PATTERN_COUNT) {
					rectfill(bmp, x1, y1, x2, y2, bgcolor);
					set_pattern(pattern);
					rectfill(bmp, x1, y1, x2, y2, fgcolor);
					clear_pattern(); 
				}
				else
					rectfill(bmp, x1, y1, x2, y2, fgcolor);
			}
			for (; border>0; border--)
				rect(bmp, x1-border, y1-border, x2+border, y2+border, fgcolor);
			break;
		case SHAPE_ROUND_RECTANGLE:
			x1 = left + border;
			y1 = top + border;
			x2 = left + width-1 - border;
			y2 = top + height-1 - border;
			if (filled) {				
				rrectfill(bmp, x1, y1, x2, y2, BORDER_RADIUS, bgcolor);
				set_pattern(pattern);
				rrectfill(bmp, x1, y1, x2, y2, BORDER_RADIUS, fgcolor);
				clear_pattern();
			}
			for (; border>0; border--)
				rrect(bmp, x1-border, y1-border, x2+border, y2+border, BORDER_RADIUS, fgcolor);
			break;
		case SHAPE_ELLIPSE:	
			x1 = left + width/2; // center x
			y1 = top + height/2; // center y
			x2 = width/2 - border; // radius x
			y2 = height/2 - border; // radius y
			if (filled) {
				ellipsefill(bmp, x1, y1, x2, y2, bgcolor);
				set_pattern(pattern);
				ellipsefill(bmp, x1, y1, x2, y2, fgcolor);
				clear_pattern();
			}
			for (; border>0; border--)
				ellipse(bmp, x1, y1, x2+border, y2+border, fgcolor);
			break;
		case SHAPE_LINE_TL_BR:
			x1 = left + (border-1)/2;
			y1 = top +(border-1)/2;
			x2 = left + width-1-border/2;
			y2 = top + height-1-border/2;
			switch(border) {
				case 3: line(bmp, x1, y1-1, x2, y2-1, fgcolor);
				case 2: line(bmp, x1, y1+1, x2, y2+1, fgcolor);
				case 1: line(bmp, x1,   y1, x2,   y2, fgcolor);
				case 0: break;
			}
			break;
		case SHAPE_LINE_BL_TR:
			x1 = left + (border-1)/2;
			y1 = top + height-1-border/2;
			x2 = left + width-1-border/2;
			y2 = top + (border-1)/2;
			for(; border>0; border--) {
				int i=border/2;
				if (border&1) i=0-i;
				line(bmp, x1, y1+i, x2, y2+i, fgcolor);
				line(bmp, x1+i, y1, x2+i, y2, fgcolor);
			}
			break;
		default:
			warning_f("Unknown shape type: %d", type);
			break;
	}
}

BITMAP *make_shape(
	int type, int width, int height, int bgcolor, int fgcolor, int border, int pattern,
	short filled
) {
	BITMAP *bmp = create_bitmap(width, height);
	if (!bmp) return NULL;
	clear_to_color(bmp, 0);
	draw_shape(bmp, 0, 0, type, width, height, bgcolor, fgcolor, border, pattern, filled);
	return bmp;
}
