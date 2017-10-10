/*
	D4Player
	* rect.c
*/

#include "rect.h"
#include "../common.h"

void read_rect(rect_t *rect, FILE *fp, short encoding) {
	rect->top    = read16(fp, encoding);
	rect->left   = read16(fp, encoding);
	rect->bottom = read16(fp, encoding);
	rect->right  = read16(fp, encoding);
}
