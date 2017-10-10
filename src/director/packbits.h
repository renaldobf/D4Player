/*
	D4Player
	* packbits.h
	
	PackBits decoder
	
	The BITD block are stored with a run-length encoding called PackBits.
	Director Images are not PackBits well-behaved, though. -128 (0x80) values are accepted.
	Further inspection is needed.
	
	The higher-than-8-bit images are arranged into color planes within each of the rows.
	24-bit images are RGB and 32-bit are aRGB, with 0xFF represent opaque.
	
*/

#ifndef _PACKBITS_H
#define _PACKBITS_H

#include <stdio.h>

int unpack_packbits_1(FILE *fp, FILE *fo, long size);
int unpack_packbits_2(FILE *fp, FILE *fo, long size);
int unpack_packbits_8(FILE *fp, FILE *fo, long size);
int unpack_packbits_24(FILE *fp, FILE *fo, long size, int width);
int unpack_packbits_32(FILE *fp, FILE *fo, long size, int width);

#endif
