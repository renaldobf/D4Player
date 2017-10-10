/*
	D4Player
	* cast.c
*/

#include "cast.h"
#include "../common.h"

#include <stdio.h>
#include <stdlib.h>

#define read16(x) read16((x), BE_ENCODING)
#define read32(x) read32((x), BE_ENCODING)

int read_cast_header(cast_t *cast, FILE *fp);
int read_cast_image(cast_t *cast, FILE *fp);
int read_cast_shape(cast_t *cast, FILE *fp);
int read_cast_script(cast_t *cast, FILE *fp);
int read_cast_additional_data(cast_t *cast, FILE *fp);

void print_cast_header(cast_t *cast);
void print_cast_image(cast_t *cast);
void print_cast_shape(cast_t *cast);

int read_cast(cast_t *cast, FILE *fp) {
	if (read_cast_header(cast, fp)) return 1;
	
	switch(cast->header.type) {
		case CAST_TYPE_IMAGE:
			if (read_cast_image(cast, fp)) {
				error("Failed to read CASt image.");
				return 1;
			}
			break;
		case CAST_TYPE_FILMLOOP:
			warning("Film loop cast is not supported");
			break;
		case CAST_TYPE_TEXT:
			warning("Text cast is not supported");
			break;
		case CAST_TYPE_PICTURE:
			warning("Picture cast is not supported");
			break;
		case CAST_TYPE_BUTTON:
			warning("Button cast is not supported");
			break;
		case CAST_TYPE_SHAPE:
			if (read_cast_shape(cast, fp)) {
				error("Failed to read CASt shape.");
				return 1;
			}
			break;
		case CAST_TYPE_MOVIE:
			warning("Movie cast is not supported");
			break;
		case CAST_TYPE_VIDEO:
			warning("Video cast is not supported");
			break;
			break;
		case CAST_TYPE_PALETTE:
		case CAST_TYPE_SOUND:
			break;
		case CAST_TYPE_SCRIPT:
			if (read_cast_script(cast, fp)) {
				error("Failed to read CASt script.");
				return 1;
			}
		default:
			warning_f("Unknown cast type found: %d", cast->header.type);
			break;
	}
	
	cast->name[0] = '\0';
	if (cast->header.additional_size > 0)
		read_cast_additional_data(cast, fp);
	
	return 0;
}

int read_cast_header(cast_t *cast, FILE *fp) {
	cast->header.header = read32(fp);
	cast->header.length = read32(fp);
	cast->header.header_size = read16(fp);
	cast->header.additional_size = read32(fp);
	cast->header.type = read8(fp);
	return 0;
}

int read_cast_image(cast_t *cast, FILE *fp) {
	cast->image.flags = read8(fp);
	cast->image.unknown01 = read16(fp);
	read_rect(&cast->image.bounds, fp, BE_ENCODING);
	read_rect(&cast->image.bounds2,fp, BE_ENCODING);
	cast->image.center_y = read16(fp);
	cast->image.center_x = read16(fp);
	if (cast->header.header_size > 24) {
		cast->image.bitdepth = read16(fp);
		cast->image.unknown02 = read16(fp);
	}
	else {
		cast->image.bitdepth = 1;
		cast->image.unknown02 = 0;
	}
	return 0;
}

int read_cast_shape(cast_t *cast, FILE *fp) {
	cast->shape.flags = read8(fp);
	cast->shape.unknown01 = read8(fp);
	cast->shape.type = read8(fp);
	read_rect(&cast->shape.bounds, fp, BE_ENCODING);
	cast->shape.pattern = read16(fp);
	cast->shape.fgcolor = read8(fp);
	cast->shape.bgcolor = read8(fp);
	cast->shape.filled = read8(fp);
	cast->shape.border = read8(fp);
	cast->shape.direction = read8(fp);
	return 0;
}

int read_cast_script(cast_t *cast, FILE *fp) {
	cast->script.flags = read8(fp);
	cast->script.unknown01 = read16(fp);
	cast->script.offset = 0;
	cast->script.size = 0;
	return 0;
}

int read_cast_additional_data(cast_t *cast, FILE *fp) {
	int32_t aditional_header_size = read32(fp);
	
	if (aditional_header_size != 20)
		// Unknown header detected
		return 0;
	
	// Reads unknwown header data
	/*Unknown*/ read32(fp);
	/*Unknown*/ read32(fp);
	/*Unknown*/ read32(fp);
	/*Unknown*/ read32(fp);
	
	int16_t record_count = read16(fp);
	/*Unknown*/ read32(fp);
	
	if (record_count < 1) return 0;
	
	int32_t *records = malloc(sizeof(int32_t) * record_count);
	
	int i;
	for (i=0; i<record_count; i++) {
		records[i] = read32(fp);
	}
	long offset = ftell(fp);
	
	// reads data...
	if (cast->header.type == CAST_TYPE_SCRIPT) {
		cast->script.offset = offset;
		cast->script.size = records[0];
	}
	
	// Reads name
	if (record_count >= 2 && records[1]-records[0] > 0) {
		fseek(fp, offset + records[0], SEEK_SET);
		int string_length = read8(fp);
		if (fread(cast->name, 1, string_length, fp) < string_length) {
			abort_on_error("Failed to read bytes from cast");
		}
		cast->name[string_length] = '\0';
	}
	return 0;
}

void print_cast(cast_t *cast) {
	print_cast_header(cast);
	switch(cast->header.type) {
		case CAST_TYPE_IMAGE:
			print_cast_image(cast);
			break;
		case CAST_TYPE_PALETTE:
		case CAST_TYPE_SOUND:
			break;
		default:
			error_f("Unknown cast type: %d.", cast->header.type);
			break;
	}
}

void print_cast_header(cast_t *cast) {
	printf(
		" == CASt ==\n"
		" header: %c%c%c%c\n"
		" length: %d\n"
		" header size: %d\n"
		" additional block size: %d\n"
		" type: %d\n",
		PRNT_TAG(cast->header.header),
		cast->header.length,
		cast->header.header_size,
		cast->header.additional_size,
		cast->header.type
	);
}

void print_cast_image(cast_t *cast) {
	printf(" == IMAGE CAST\n");
	printf(" flags: %d (%d%d%d%d%d%d%d%d)\n",
		cast->image.flags,
		!!(cast->image.flags&128),
		!!(cast->image.flags&64),
		!!(cast->image.flags&32),
		!!(cast->image.flags&16),
		!!(cast->image.flags&8),
		!!(cast->image.flags&4),
		!!(cast->image.flags&2),
		!!(cast->image.flags&1)
	);
	printf(" unknown 16bit: %04X - Flags: %d %d %d %d %d - Width: %d\n",
		(cast->image.unknown01),
		!!((cast->image.unknown01>>15)&1),
		!!((cast->image.unknown01>>14)&1),
		!!((cast->image.unknown01>>13)&1),
		!!((cast->image.unknown01>>12)&1),
		!!((cast->image.unknown01>>11)&1),
		(cast->image.unknown01)&0x7FF
	);
	printf(" bounds: %d %d %d %d (%d x %d)\n",
		cast->image.bounds.top,
		cast->image.bounds.left,
		cast->image.bounds.bottom,
		cast->image.bounds.right,
		cast->image.bounds.right - cast->image.bounds.left,
		cast->image.bounds.bottom - cast->image.bounds.top
	);
	printf(" bounds2: %d %d %d %d\n",
		cast->image.bounds2.top,
		cast->image.bounds2.left,
		cast->image.bounds2.bottom,
		cast->image.bounds2.right
	);
	printf(" center: %d %d\n",
		cast->image.center_x,
		cast->image.center_y
	);
	printf(" bitdepth: %d\n", cast->image.bitdepth);
	printf(" unknown 16bit: %d\n", cast->image.unknown02);
}
