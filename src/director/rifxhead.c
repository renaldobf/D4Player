/*
	D4Player
	* rifxhead.c
*/

#include "rifxhead.h"
#include "../common.h"

#include <stdio.h>
#include <string.h>

int read_rifx_header(rifx_header_t *header, FILE *fp) {
	int items_read = fread(header, RIFX_HEADER_SIZE, 1, fp);
	if (items_read < 1) {
		if (ferror(fp))
			error("Error while reading file header");
		else if (feof(fp))
			error("End of file reachead while reading file header");
		else
			error("Could not read the file header for unknown causes");
		return 1;
	}
	
	short encoding = BE_ENCODING;
	I32_FROM_BE(header->header);
	
	if (header->header == MKTAG(RIFX_HEADER_4CC)) {
		/* Empty */
	}
	else {
		SWAP_32(header->header);
		if (header->header == MKTAG(RIFX_HEADER_4CC)) {
			warning("Little endian encoded file");
			encoding = LE_ENCODING;
		}
		else {
			error("File is not a RIFX archive");
			return -1;
		}
	}
	
	CONVERT_32(encoding, header->length);
	CONVERT_32(encoding, header->version);
		
	if (header->version != MKTAG("MV93")) {
		error_f("The file version is not supported: %c%c%c%c", PRNT_TAG(header->version));
		return -1;
	}
	
	return encoding;
}

void print_rifx_header(rifx_header_t *header) {
	printf(
		"-- RIFX HEADER\n"
		" header: %c%c%c%c\n"
		" length: %d\n"
		" version: %c%c%c%c\n"
		"\n",
		PRNT_TAG(header->header),
		header->length,
		PRNT_TAG(header->version)
	);
}
