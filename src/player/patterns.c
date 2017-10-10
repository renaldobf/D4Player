/*
	D4Player
	* patterns.c
*/

#include "patterns.h"
#include "allegro.h"
#include "../common.h"

BITMAP *patterns[PATTERN_COUNT];

void load_patterns() {
	char filename[255] ;
	int i;
	for (i=0; i<PATTERN_COUNT; i++) {
		sprintf(filename, "patterns/%02d.tga", i+1);
		patterns[i] = load_bitmap(filename, NULL);
		if (!patterns[i]) abort_on_error("Error loading pattern file");
	}
}

void free_patterns() {
	int i;
	for (i=0; i<PATTERN_COUNT; i++) {
		if (!patterns[i]) continue;
		destroy_bitmap(patterns[i]);
		patterns[i] = NULL;
	}
}

void set_pattern(int index) {
	if (index >= 1 && index <= PATTERN_COUNT)
		drawing_mode(DRAW_MODE_MASKED_PATTERN, patterns[index-1], 0, 0);
	else
		clear_pattern();
}

void clear_pattern() {
	drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
}
