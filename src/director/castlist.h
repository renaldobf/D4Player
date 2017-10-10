/*
	D4Player
	* castlist.h
	
	== CAS* ==
	One-base index of CASt members.
	Each entry, a 32bit integer, makes reference to the memory map location of the CASt.
	Some entries are 0, which indicates no member at that position.
*/

#ifndef _CASTLIST_H
#define _CASTLIST_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
	int32_t header;
	int32_t length;
} castlist_header_t;

#define CAST_LIST_HEADER_SIZE sizeof(castlist_header_t)

typedef struct {
	castlist_header_t header;
	int32_t *entries;
	int32_t size;
} castlist_t;

#define CAST_LIST_ENTRY_SIZE 4

int read_castlist(castlist_t *castlist, FILE *fp, short encoding);
void free_castlist(castlist_t *castlist);
void print_castlist_header(castlist_t *castlist);
void print_castlist(castlist_t *castlist);

#endif
