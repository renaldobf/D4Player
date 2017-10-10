/*
	D4Player
	* keys.c
*/

#include "keys.h"
#include "../common.h"
#include "mmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int read_keys(keys_t *keys, FILE *fp, short encoding) {
	int records_read = fread(&keys->header, sizeof(keys_header_t), 1, fp);
	if (records_read < 1) {
		if (ferror(fp))
			error("Error while reading KEY* block.");
		else if (feof(fp))
			error("End of file reachead while reading KEY* block.");
		else
			error("Could not read the KEY* entry for unknown block.");
		return 1;
	}
	CONVERT_32(encoding, keys->header.header);
	CONVERT_32(encoding, keys->header.length);
	CONVERT_16(encoding, keys->header.header_size);
	CONVERT_16(encoding, keys->header.record_size);
	CONVERT_32(encoding, keys->header.record_count);
	CONVERT_32(encoding, keys->header.active_records);
	
	keys->entries = malloc(keys->header.active_records*sizeof(keys_entry_t));
	if (!keys->entries) {
		error("Error allocating memory for the KEY* entries.");
		return 1;
	}
	keys->size = 0;
	
	while (keys->size < keys->header.active_records) {
		if (read_keys_entry(&keys->entries[keys->size], fp, encoding)) {
			free_keys(keys);
			return 1;
		}
		
		keys->size++;
	}
	
	return 0;
}

int read_keys_entry(keys_entry_t *entry, FILE *fp, short encoding) {
	int records_read = fread(entry, sizeof(keys_entry_t), 1, fp);
	if (records_read < 1) {
		if (ferror(fp))
			error("Error while reading KEY* entry.");
		else if (feof(fp))
			error("End of file reachead while reading KEY* entry.");
		else
			error("Could not read the KEY* entry for unknown causes.");
		return 1;
	}
	CONVERT_32(encoding, entry->elem_index);
	CONVERT_32(encoding, entry->cast_index);
	CONVERT_32(encoding, entry->type);
	
	return 0;
}

void free_keys(keys_t *keys) {
	if (!keys) return;
	keys->size = 0;
	if (keys->entries) {
		free(keys->entries);
		keys->entries = NULL;
	}
}

void print_keys_header(keys_t *keys) {
	printf(
		"-- KEY*\n"
		" header: %c%c%c%c\n"
		" length: %d\n"
		" header size: %d\n"
		" record size: %d\n"
		" record count: %d\n"
		" active records: %d\n"
		"\n",
		PRNT_TAG(keys->header.header),
		keys->header.length,
		keys->header.header_size,
		keys->header.record_size,
		keys->header.record_count,
		keys->header.active_records
	);
}

void print_key(keys_t *keys, int32_t index) {
	if (!keys || !keys->entries || index < 0 || index >= keys->size) {
		error_f("Invalid index for keys: %d", index);
		return;
	}
	keys_entry_t *entry = keys->entries+index;
	printf(
		"-- KEY* entry\n"
		" index: %d\n"
		" element index: %d\n"
		" cast index: %d\n"
		" element type: %c%c%c%c\n",
		index,
		entry->elem_index,
		entry->cast_index,
		PRNT_TAG(entry->type)
	);
}

void list_keys(keys_t *keys) {
	if (!keys || !keys->entries) return;
	
	printf(" index |  elem |  cast | type \n");
	printf("------------------------------\n");
	
	keys_entry_t *entry = keys->entries;
	int32_t i;
	for (i=0; i<keys->size; i++) {
		printf(" %5d | %5d | %5d | %c%c%c%c \n",
			i,
			entry->elem_index,
			entry->cast_index,
			PRNT_TAG(entry->type)
		);
		entry++;
	}
}

int32_t keys_search(keys_t *keys, const char *type, int32_t start) {
	
	if (!keys || !keys->entries || start < 0 || start >= keys->size)
		return -1;
	
	keys_entry_t *entry = keys->entries + start;
	int32_t i;
	for (i=start; i<keys->size; i++) {
		if (entry->type == MKTAG(type))
			return i;
		entry++;
	}
	return -1;
}

int32_t keys_get_cast(keys_t *keys, int32_t index) {
	
	if (!keys || !keys->entries || index < 0)
		return -1;
	
	keys_entry_t *entry = keys->entries;
	int32_t i;
	for (i=0; i<keys->size; i++) {
		if (entry->elem_index == index)
			return entry->cast_index;
		entry++;
	}
	return -1;
}

int32_t keys_get_elem(keys_t *keys, int32_t index, const char *type) {
	
	if (!keys || !keys->entries || index < 0)
		return -1;
	
	keys_entry_t *entry = keys->entries;
	int32_t i;
	for (i=0; i<keys->size; i++) {
		if (entry->cast_index == index && (type == NULL || entry->type == MKTAG(type)))
			return entry->elem_index;
		entry++;
	}
	return -1;
}
