/*
	D4Player
	* rifxhead.h
	
	== RIFX == 
	It's the file header.
	
	* 4 bytes char - header indicating the format (RIFX)
	* 32 bit integer - block size
	* 4 bytes char - version of the file (MV93)

*/

#ifndef _RIFXHEAD_H
#define _RIFXHEAD_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
	int32_t header;
	int32_t length;
	int32_t version;
} rifx_header_t;

#define RIFX_HEADER_SIZE (sizeof(rifx_header_t))
#define RIFX_HEADER_4CC "RIFX"

int read_rifx_header(rifx_header_t *header, FILE *fp);

void print_rifx_header(rifx_header_t *header);

#endif
