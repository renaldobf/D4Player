/*
	D4Player
	* movieinf.c
*/

#include "movieinf.h"
#include "../common.h"
#include "rect.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int read_movieinfo(movieinfo_t *info, FILE *fp, short encoding) {
	if (
		fread(&info->header, sizeof(vwcf_header_t), 1, fp) < 1 ||
		fread(&info->rect, 8, 1, fp) < 1
	) {
		error("Error while reading movie information block");
		return 1;
	}
	
	CONVERT_32(encoding, info->header.header);
	CONVERT_32(encoding, info->header.length);
	CONVERT_16(BE_ENCODING, info->header.unknown01);
	CONVERT_16(BE_ENCODING, info->header.version);
	
	CONVERT_16(BE_ENCODING, info->rect.top);
	CONVERT_16(BE_ENCODING, info->rect.left);
	CONVERT_16(BE_ENCODING, info->rect.bottom);
	CONVERT_16(BE_ENCODING, info->rect.right);
	
	info->width = info->rect.right - info->rect.left;
	info->height = info->rect.bottom - info->rect.top;
	
	info->castlist_start = read16(fp, BE_ENCODING);
	info->castlist_end = read16(fp, BE_ENCODING);
	
	info->framerate = read8(fp);
	
	fseek(fp, 9, SEEK_CUR);
	
	info->bgcolor = read16(fp, BE_ENCODING);
	
	return 0;
}

void print_movieinfo(movieinfo_t *info) {
	printf(
		" --VWCF\n"
		" header: %c%c%c%c\n"
		" length: %d\n"
		" unk01: %d\n"
		" version: %d\n"
		" -- Movie rect\n"
		" top: %d\n"
		" left: %d\n"
		" bottom: %d\n"
		" right: %d\n"
		" ( %d x %d )\n"
		" Castlist index from %d to %d\n"
		" Initial framerate: %d\n"
		" Background color index: %d\n"
		"\n",
		PRNT_TAG(info->header.header),
		info->header.length,
		info->header.unknown01,
		info->header.version,
		info->rect.top,
		info->rect.left,
		info->rect.bottom,
		info->rect.right,
		info->width, info->height,
		info->castlist_start,
		info->castlist_end,
		info->framerate,
		info->bgcolor
	);
}
