/*
	D4Player
	* palette.c
*/

#include "palette.h"
#include "../common.h"

// Loads a pallete from file 
void load_palette_file(palette_t *palette, const char *filename) {
	FILE *file = fopen(filename,"r");
	if (!file) abort_on_error("Failed to open palette file");
	palette->size = fread(palette->palette, 3, PAL_SIZE, file);
	int i;
	for (i=palette->size; i<PAL_SIZE; i++) {
		palette->palette[i] = (pixel24_t) {0,0,0};
	}
	fclose(file);
}
