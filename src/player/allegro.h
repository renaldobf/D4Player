/*
	D4Player
	* allegro.h
*/

#ifndef _ALLEGRO_H
#define _ALLEGRO_H

// <stdint.h> must be included before Allegro.
// On DJGPP under DOS, Allegro redefines new int types who are "too long".
// If <stdint.h> is allready included, Allegro doesn't redefine them.
#include <stdint.h>
// "../common.h" must be included before Allegro.
// Allegro defines BIG_ENDIAN, which should not be defined before common.h is included.
#include "../common.h"

#include <allegro.h>

#define COLOR_DEPTH 8
#define WINDOW_TITLE "Player"

#define FALLBACK_RESOLUTION_LIST_W (int[])({320,320,640,768,800,1024})
#define FALLBACK_RESOLUTION_LIST_H (int[])({200,240,480,480,600,768})
#define FALLBACK_RESOLUTION_LIST_SIZE (6)

extern BITMAP *buffer;
extern void (*flush_buffer)();
extern void (*clone_buffer)();

int start_allegro(int width, int height);
void stop_allegro();
void rrect(BITMAP *bmp, int x1, int y1, int x2, int y2, int r, int color);
void rrectfill(BITMAP *bmp, int x1, int y1, int x2, int y2, int r, int color);

#endif
