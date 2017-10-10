/*
	D4Player
	* imgwrap.h
*/

#ifndef _IMGWRAP_H
#define _IMGWRAP_H
#include <stdio.h>

#if !defined(ENABLE_PNG)
	#define ENABLE_PNG 1
#endif

extern int (*write_img)(FILE*, unsigned char*, int, int, int, unsigned char *, int);

int write_png(
	FILE *fp, unsigned char *buffer, int width, int height, int color_depth,
	unsigned char *palette, int palette_size
);
int write_ppm(
	FILE *fp, unsigned char *buffer, int width, int height, int color_depth,
	unsigned char *palette, int palette_size
);

#endif
