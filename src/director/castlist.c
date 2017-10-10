/*
	D4Player
	* castlist.c
*/

#include "castlist.h"
#include "../common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int read_castlist(castlist_t *castlist, FILE *fp, short encoding) {
	int records_read = fread(&castlist->header, CAST_LIST_HEADER_SIZE, 1, fp);
	if (records_read < 1) {
		if (ferror(fp))
			error("Error while reading CAS* block.");
		else if (feof(fp))
			error("End of file reachead while reading CAS* block.");
		else
			error("Could not read the CAS* block for unknown block.");
		return 1;
	}
	CONVERT_32(encoding, castlist->header.header);
	CONVERT_32(encoding, castlist->header.length);
	
	int entry_count = castlist->header.length / CAST_LIST_ENTRY_SIZE;
	
	castlist->entries = malloc(entry_count * CAST_LIST_ENTRY_SIZE);
	if (!castlist->entries) {
		error("Could not allocate memory for the CAS* list.");
		return 1;
	}
	castlist->size = 0;
	
	while(castlist->size < entry_count) {
		records_read = fread(&castlist->entries[castlist->size], CAST_LIST_ENTRY_SIZE, 1, fp);
		if (records_read < 1) {
			if (ferror(fp))
				error("Error while reading CAS* entry.");
			else if (feof(fp))
				error("End of file reachead while reading CAS* entry.");
			else
				error("Could not read the CAS* entry for unknown block.");
			
			free_castlist(castlist);
			return 1;
		}
		
		CONVERT_32(BE_ENCODING, castlist->entries[castlist->size]);
		castlist->size++;
	}
	
	return 0;
}

void free_castlist(castlist_t *castlist) {
	if (!castlist) return;
	
	castlist->size = 0;
	if (castlist->entries) {
		free(castlist->entries);
		castlist->entries = NULL;
	}
}

void print_castlist_header(castlist_t *castlist) {
	if (!castlist) return;
	printf(
		"-- CAS*\n"
		" header: %c%c%c%c\n"
		" length: %d\n"
		"\n"
		"-- %d entries total",
		PRNT_TAG(castlist->header.header),
		castlist->header.length,
		castlist->size
	);
}

void print_castlist(castlist_t *castlist) {
	if (!castlist || !castlist->entries) return;
	printf(" cast | index \n");
	printf("--------------\n");
	int32_t i;
	for (i=0; i<castlist->size; i++) {
		if (castlist->entries[i])
			printf(" %4d | %5d \n", i+1, castlist->entries[i]);
		else
			printf(" %4d |     - \n", i+1);
	}
}
