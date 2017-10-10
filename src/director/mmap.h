/*
	D4Player
	* mmap.h
	
	== MMAP ==
	A memory map, showing the position and size of the blocks in the file.
	The free and junk entries form a linked list of the empty spaces on the map and junk on the file
	respectively.
	
	Starts with the header:
	
	* 4 bytes char - name of the block (mmap)
	* 32 bit integer - length of the block
	* 16 bit integer - header size (24)
	* 16 bit integer - size of each entry (20)
	* 32 bit integer - number of entries
	* 32 bit integer - number of non-zero entries
	* 32 bit integer - first free entry index on the table (zero-based index)
	* 32 bit integer - first junk entry index on the table (zero-based index)
	* 32 bit integer - first junk entry index on the table (zero-based index)
	
	Then comes the entries, 20 bytes each, with the following structure:
	
	* 4 bytes char - block type
	* 32 bit integer - block size
	* 32 bit integer - padding from the start of the file
	* 16 bit integer - unknown (1 for the already read blocks, 12 for free entries, 4 or 5 for junk, 0 otherwise)
	* 16 bit integer - unknown (allways 0)
	* 32 bit integer - unknown (see below)
	
	This last values has different values:
	* For the imap and mmap, have a still undecoded number
	* For the free and junk entries, has the index of the next entry on the list or -1 for the last element.
	* For the rest of the entries, it's 0.

*/

#ifndef _MMAP_H
#define _MMAP_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
	int32_t header;
	int32_t length;
	int16_t offset;
	int16_t record_size;
	int32_t record_count;
	int32_t active_records;
	int32_t junk_index1;
	int32_t junk_index2;
	int32_t free_index;
} mmap_header_t;

#define MMAP_HEADER_SIZE 32

typedef struct {
	int32_t type;
	int32_t length;
	int32_t offset;
	int16_t unknown1;
	int16_t unknown2;
	int32_t unknown3;
} mmap_entry_t;

typedef struct {
	mmap_header_t header;
	mmap_entry_t *entries;
	int32_t size;
} mmap_t;

#define MMAP_4CC "mmap"

int read_mmap(mmap_t *mmap, FILE *fp, short encoding);
int read_mmap_entry(mmap_entry_t *entry, FILE *fp, short encoding);
void free_mmap(mmap_t *mmap);
void print_mmap_header(mmap_t *mmap);
void print_mmap_entry(mmap_t *mmap, int32_t index);
void print_mmap_entries(mmap_t *mmap);
int32_t mmap_search(mmap_t *mmap, const char *type, int32_t start);
int32_t mmap_get_offset(mmap_t *mmap, int32_t index);
int32_t mmap_get_length(mmap_t *mmap, int32_t index);

#endif
