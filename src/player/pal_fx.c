/*
	D4Player
	* pal_fx.c
*/

#include "pal_fx.h"
#include "allegro.h"
#include "../director/palette.h"

// Sets the current palette
void set_current_palette(palette_t *palette) {
	PALETTE allegro_pal;
	int i;
	// Downsample pallete to 18 bits (Allegro default / VGA)
	for (i=0; i<palette->size; i++) {
		allegro_pal[i].r = palette->palette[i].r >> 2;
		allegro_pal[i].g = palette->palette[i].g >> 2;
		allegro_pal[i].b = palette->palette[i].b >> 2;
	}
	for (; i<PAL_SIZE; i++)
		allegro_pal[i] = (RGB){0,0,0};
	set_palette(allegro_pal);
}
