/*
	D4Player
	* patterns.h
*/

#ifndef _PATTERNS_H
#define _PATTERNS_H

#include "allegro.h"

#define PATTERN_COUNT 16

extern BITMAP *patterns[PATTERN_COUNT];

void load_patterns();
void free_patterns();
void set_pattern(int index);
void clear_pattern();

#endif
