/*
	D4Player
	* effects.c
*/

#include "effects.h"
#include "allegro.h"

COLOR_MAP g_table_blend;
COLOR_MAP g_table_ghost;
COLOR_MAP g_table_reverse;
COLOR_MAP g_table_lightest;
COLOR_MAP g_table_darkest;
COLOR_MAP g_table_add;
COLOR_MAP g_table_add_pin;
COLOR_MAP g_table_subtract;
COLOR_MAP g_table_sub_pin;
COLOR_MAP g_table_tint;

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef CAPS
#define CAPS(a,b,x) ((x)<(a)?(a):((x)>(b)?(b):(x)))
#endif

void return_ghost(const PALETTE pal, int x, int y, RGB *rgb) {
	RGB b = pal[y];
	*rgb = (RGB){
		(x>0) ? (63) : (b.r),
		(x>0) ? (63) : (b.g),
		(x>0) ? (63) : (b.b),
	};
}

void return_reverse(const PALETTE pal, int x, int y, RGB *rgb) {
	RGB b = pal[y];
	*rgb = (RGB){
		(x>0) ? (63-b.r) : (b.r),
		(x>0) ? (63-b.g) : (b.g),
		(x>0) ? (63-b.b) : (b.b),
	};
}

void return_lightest(const PALETTE pal, int x, int y, RGB *rgb) {
	RGB a = pal[x], b = pal[y];
	*rgb = (RGB){
		MAX(a.r,b.r),
		MAX(a.g,b.g),
		MAX(a.b,b.b)
	};
}

void return_darkest(const PALETTE pal, int x, int y, RGB *rgb) {
	RGB a = pal[x], b = pal[y];
	*rgb = (RGB){
		MIN(a.r,b.r),
		MIN(a.g,b.g),
		MIN(a.b,b.b)
	};
}

void return_add(const PALETTE pal, int x, int y, RGB *rgb) {
	RGB a = pal[x], b = pal[y];
	*rgb = (RGB){
		((int) a.r + b.r)%64,
		((int) a.g + b.g)%64,
		((int) a.b + b.b)%64
	};
}

void return_add_pin(const PALETTE pal, int x, int y, RGB *rgb) {
	RGB a = pal[x], b = pal[y];
	*rgb = (RGB){
		CAPS(0, 63, (int) a.r + b.r),
		CAPS(0, 63, (int) a.g + b.g),
		CAPS(0, 63, (int) a.b + b.b)
	};
}

void return_subtract(const PALETTE pal, int x, int y, RGB *rgb) {
	RGB a = pal[x], b = pal[y];
	*rgb = (RGB){
		((int) b.r - a.r)%64,
		((int) b.g - a.g)%64,
		((int) b.b - a.b)%64
	};
}

void return_sub_pin(const PALETTE pal, int x, int y, RGB *rgb) {
	RGB a = pal[x], b = pal[y];
	*rgb = (RGB){
		CAPS(0, 63, (int) b.r - a.r),
		CAPS(0, 63, (int) b.g - a.g),
		CAPS(0, 63, (int) b.b - a.b)
	};
}

void return_tint(const PALETTE pal, int x, int y, RGB *rgb) {
	if (x == 0)
		*rgb = pal[y];
	else
		*rgb = pal[x];
}

void create_effects() {
	PALETTE pal;
	get_palette(pal);
	create_color_table(&g_table_ghost   , pal, return_ghost   , NULL);
	create_color_table(&g_table_reverse , pal, return_reverse , NULL);
	create_color_table(&g_table_lightest, pal, return_lightest, NULL);
	create_color_table(&g_table_darkest , pal, return_darkest , NULL);
	create_color_table(&g_table_add     , pal, return_add     , NULL);
	create_color_table(&g_table_add_pin , pal, return_add_pin , NULL);
	create_color_table(&g_table_subtract, pal, return_subtract, NULL);
	create_color_table(&g_table_sub_pin , pal, return_sub_pin , NULL);
	create_trans_table(&g_table_blend   , pal, 127, 127, 127, NULL);
	create_color_table(&g_table_tint    , pal, return_tint    , NULL);
}
