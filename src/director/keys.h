/*
	D4Player
	* keys.h
	
	== KEY* ==
	Association table between element and a cast member.
	Each entry is composed by two 32bit integers, the mmap index of the element and the cast, followed by the element type.
	The rest of the block are non-used records, no usable data, probably uncleared memory written to disk.
	
	header:
	* 4 bytes char - name of the block (KEY*)
	* 32 bit integer - length of the block
	* 16 bit integer - header size (12)
	* 16 bit integer - record size (12)
	* 32 bit integer - record count
	* 32 bit integer - active records
	
	entries:
	* 32 bit integer - the element index
	* 32 bit integer - associated CASt index
	* 4 bytes char - block type
	
	The following elements are not associated to a CASt.
	They are listed as having the second index value of 1024.
	* CAS*
	* FXmp
	* Fmap
	* Lctx
	* Sord
	* VWCF
	* VWFI
	* VWFM
	* VWLB
	* VWSC
	* VWtk

*/

#ifndef _KEYS_H
#define _KEYS_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
	int32_t header;
	int32_t length;
	int16_t header_size;
	int16_t record_size;
	int32_t record_count;
	int32_t active_records;
} keys_header_t;

typedef struct {
	int32_t elem_index;
	int32_t cast_index;
	int32_t type;
} keys_entry_t;

typedef struct {
	keys_header_t header;
	keys_entry_t *entries;
	int32_t size;
}  keys_t;

int read_keys(keys_t *keys, FILE *fp, short encoding);
int read_keys_entry(keys_entry_t *entry, FILE *fp, short encoding);
void free_keys(keys_t *keys);
void print_keys_header(keys_t *keys);
void print_key(keys_t *keys, int32_t index);
void list_keys(keys_t *keys);
int32_t keys_search(keys_t *keys, const char *type, int32_t start);
int32_t keys_get_cast(keys_t *keys, int32_t index);
int32_t keys_get_elem(keys_t *keys, int32_t index, const char *type);

#endif
