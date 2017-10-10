/*
	D4Player
	* mmap.c
*/

#include "mmap.h"
#include "../common.h"

#include <stdio.h>
#include <stdlib.h>

int read_mmap(mmap_t *mmap, FILE *fp, short encoding) {
	int bytes_read = fread(&mmap->header, MMAP_HEADER_SIZE, 1, fp);
	if (bytes_read < 1) {
		if (ferror(fp))
			error("Error while reading mmap block");
		else if (feof(fp))
			error("End of file reachead while reading mmap block");
		else
			error("Could not read mmap block for unknown causes");
		return 1;
	}
	
	CONVERT_32(encoding, mmap->header.header);
	
	if (mmap->header.header != MKTAG(MMAP_4CC)) {
		error_f("Invalid header for mmap block: %c%c%c%c",
			PRNT_TAG(mmap->header.header)
		);
		return 1;
	}
	
	CONVERT_32(encoding, mmap->header.length);
	CONVERT_16(encoding, mmap->header.offset);
	CONVERT_16(encoding, mmap->header.record_size);
	CONVERT_32(encoding, mmap->header.record_count);
	CONVERT_32(encoding, mmap->header.active_records);
	CONVERT_32(encoding, mmap->header.junk_index1);
	CONVERT_32(encoding, mmap->header.junk_index2);
	CONVERT_32(encoding, mmap->header.free_index);
	
	mmap->size = 0;
	mmap->entries = malloc(sizeof(mmap_entry_t) * mmap->header.active_records);
	if (!mmap->entries) {
		error("Could not allocate memory for memory map.\n");
		return 1;
	}
	
	while (mmap->size < mmap->header.active_records) {
		if (read_mmap_entry(&mmap->entries[mmap->size], fp, encoding)) {
			free_mmap(mmap);
			return 1;
		}
		mmap->size++;
	}
	
	return 0;
}

int read_mmap_entry(mmap_entry_t *entry, FILE *fp, short encoding) {
	int bytes_read = fread(entry,20,1,fp);
	if (bytes_read < 1) {
		if (ferror(fp))
			error("Error while reading mmap entry");
		else if (feof(fp))
			error("End of file reachead while reading mmap entry");
		else
			error("Could not read mmap entry for unknown causes");
		return 1;
	}
	CONVERT_32(encoding, entry->type);
	CONVERT_32(encoding, entry->length);
	CONVERT_32(encoding, entry->offset);
	CONVERT_16(encoding, entry->unknown1);
	CONVERT_16(encoding, entry->unknown2);
	CONVERT_32(encoding, entry->unknown3);
	
	return 0;
}

void free_mmap(mmap_t *mmap) {
	if (!mmap) return;
	mmap->size = 0;
	if (mmap->entries) {
		free(mmap->entries);
		mmap->entries = NULL;
	}
}

void print_mmap_header(mmap_t *mmap) {
	printf(
		"-- MMAP\n"
		" header: %c%c%c%c\n"
		" length: %d\n"
		" header size: %d\n"
		" record size: %d\n"
		" record count: %d\n"
		" active records: %d\n"
		" junk index 1: %d\n"
		" junk index 2: %d\n"
		" free index: %d\n"
		"\n",
		PRNT_TAG(mmap->header.header),
		mmap->header.length,
		mmap->header.offset,
		mmap->header.record_size,
		mmap->header.record_count,
		mmap->header.active_records,
		mmap->header.junk_index1,
		mmap->header.junk_index2,
		mmap->header.free_index
	);
}

void print_mmap_entry(mmap_t *mmap, int32_t index) {
	if (!mmap || !mmap->entries || index < 0 || index >= mmap->size) {
		error_f("Invalid index for mmap: %d", index);
		return;
	}
	mmap_entry_t *entry = mmap->entries + index;
	printf(
		"-- MMAP entry\n"
		" index: %d\n"
		" type: %c%c%c%c\n"
		" size: %d\n"
		" offset: %d\n"
		" f01: %d\n"
		" f02: %d\n"
		" f03: %d\n",
		index,
		PRNT_TAG(entry->type),
		entry->length,
		entry->offset,
		entry->unknown1,
		entry->unknown2,
		entry->unknown3
	);
}

void print_mmap_entries(mmap_t *mmap) {
	printf(
		" index | type |  length  |  offset  |  f01  |  f02  |   f03    \n"
		"---------------------------------------------------------------\n");
	
	mmap_entry_t *entry = mmap->entries;
	int32_t i;
	for (i=0; i<mmap->size; i++) {
		
		printf((entry->unknown3 < 1000000 ?
			"  %4d | %c%c%c%c | %8d | %8d | %5d | %5d | %8d  \n":
			"  %4d | %c%c%c%c | %8d | %8d | %5d | %5d | %08X  \n"),
			i,
			PRNT_TAG(entry->type),
			entry->length,
			entry->offset,
			entry->unknown1,
			entry->unknown2,
			entry->unknown3
		);
		entry++;
	}
	printf("\n");
}

int32_t mmap_search(mmap_t *mmap, const char *type, int32_t start) {
	
	if (!mmap || !mmap->entries || start < 0 || start >= mmap->size)
		return -1;
	
	mmap_entry_t *entry = mmap->entries + start;
	int32_t i;
	for (i=start; i<mmap->size; i++) {
		if (entry->type == MKTAG(type))
			return i;
		entry++;
	}
	return -1;
}

int32_t mmap_get_offset(mmap_t *mmap, int32_t index) {
	if (!mmap || !mmap->entries || index < 0 || index >= mmap->size)
		return -1;
	return mmap->entries[index].offset;
}


int32_t mmap_get_length(mmap_t *mmap, int32_t index) {
	if (!mmap || !mmap->entries || index < 0 || index >= mmap->size)
		return -1;
	return mmap->entries[index].length;
}
