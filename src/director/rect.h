/*
	D4Player
	* rect.c
*/

#ifndef _RECT_H
#define _RECT_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
	int16_t top;
	int16_t left;
	int16_t bottom;
	int16_t right;
} rect_t;

void read_rect(rect_t *rect, FILE *fp, short encoding);

#endif
