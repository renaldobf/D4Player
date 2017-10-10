/*
	D4Player
	* imap.h
	
	== IMAP == 
	The IMAP block appears to have constant values across files.
	It's known to have the following fields:
	
	* 4 bytes char - name of the block (imap)
	* 32 bit integer - length of the block (24)
	* 32 bit integet - unknown (allways 1)
	* 32 bit integer - padding to the MMAP block from the start of the file (44)
	* 32 bit integer - unknown (allways 0)
	* 32 bit integer - unknown (allways 0)
	* 32 bit integer - unknown (allways 0)
	* 32 bit integer - unknown (allways 0)

*/

#ifndef _IMAP_H
#define _IMAP_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
	int32_t header;
	int32_t length;
	uint32_t unknown1;
	uint32_t offset;
	uint32_t unknown2;
	uint32_t unknown3;
	uint32_t unknown4;
	uint32_t unknown5;
} imap_t;

#define IMAP_SIZE (sizeof(imap_t))
#define IMAP_4CC "imap"

int read_imap(imap_t *imap, FILE *fp, short encoding);
void print_imap(imap_t *imap);

#endif
