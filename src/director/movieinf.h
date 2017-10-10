/*
	D4Player
	* movieinf.h
	
	== VWCF ==
	VideoWorks configuration. Partial support.
	
	Most of decoded information about this block is from Continuity
	File version is not being checked
*/

#ifndef _MOVIEINF_H
#define _MOVIEINF_H

#include "rect.h"

#include <stdio.h>
#include <stdint.h>

typedef struct {
	uint32_t header;
	uint32_t length;
	uint16_t unknown01;
	uint16_t version;
} vwcf_header_t;

typedef struct {
	vwcf_header_t header;
	rect_t rect;
	int width;
	int height;
	int castlist_start;
	int castlist_end;
	int framerate;
	int bgcolor;
} movieinfo_t;

int read_movieinfo(movieinfo_t *info, FILE *fp, short encoding);
void print_movieinfo(movieinfo_t *info);

#endif
