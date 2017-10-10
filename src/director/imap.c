/*
	D4Player
	* imap.c
*/

#include "imap.h"
#include "../common.h"

#include <stdio.h>
#include <string.h>

int read_imap(imap_t *imap, FILE *fp, short encoding) {
	int records_read = fread(imap, IMAP_SIZE, 1, fp);
	if (records_read < 1) {
		if (ferror(fp))
			error("Error while reading imap block.");
		else if (feof(fp))
			error("End of file reachead while reading imap block.");
		else
			error("Could not read the imap block for unknown causes.");
		return 1;
	}
	
	CONVERT_32(encoding, imap->header);
	CONVERT_32(encoding, imap->length);
	CONVERT_32(encoding, imap->unknown1);
	CONVERT_32(encoding, imap->offset);
	CONVERT_32(encoding, imap->unknown2);
	CONVERT_32(encoding, imap->unknown3);
	CONVERT_32(encoding, imap->unknown4);
	CONVERT_32(encoding, imap->unknown5);
	
	if (imap->header != MKTAG(IMAP_4CC)) {
		error("Invalid header for imap block");
		return 1;
	}
	
	return 0;
}

void print_imap(imap_t *imap) {
	printf(
		"-- IMAP\n"
		" header: %c%c%c%c\n"
		" length: %d\n"
		" unk_01: %d\n"
		" offset: %d\n"
		" unk_02: %d\n"
		" unk_03: %d\n"
		" unk_04: %d\n"
		" unk_05: %d\n"
		"\n",
		PRNT_TAG(imap->header),
		imap->length,
		imap->unknown1,
		imap->offset,
		imap->unknown2,
		imap->unknown3,
		imap->unknown4,
		imap->unknown5
	);
}
