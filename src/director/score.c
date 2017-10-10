/*
	D4Player
	* score.h
*/ 

#include "score.h"
#include "../common.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

void print_frame(FILE *fp, int index, uint16_t *frame, int size) {
	int i;
	fprintf(fp,"%d",index+1);
	for (i=0;i<size;i++) {
		fprintf(fp,",%d",frame[i]);
	}
	fprintf(fp,"\n");
}

int read_score(score_t *score, FILE *fp, short encoding) {
	vwsc_header_t vwsc_header;
	int bytes_read = fread(&vwsc_header, VWSC_HEADER_SIZE, 1, fp);
	if (bytes_read < 1) {
		error("Failed to read score");
		return 1;
	}
	
	CONVERT_32(encoding, vwsc_header.header);
	CONVERT_32(encoding, vwsc_header.length);
	
	if (vwsc_header.header != MKTAG(SCORE_4CC)) {
		abort_on_error_f("Invalid header for score block: %c%c%c%c",
			PRNT_TAG(vwsc_header.header)
		);
	}
	
	int32_t score_size = read32(fp, BE_ENCODING);
	int32_t score_bytes_read = 0;
	int32_t header_size = read32(fp, BE_ENCODING);
	int32_t frame_count = read32(fp, BE_ENCODING);
	
	if (header_size == -3) {
		int entry_count = read32(fp, BE_ENCODING);
		/* entry_count+1 */ read32(fp, BE_ENCODING);
		/* total_entry_size */ read32(fp, BE_ENCODING);
		long offset  = read32(fp, BE_ENCODING); // Offset to frame data after the entries offset
		offset = offset + entry_count * 4;
		fseek(fp, offset, SEEK_CUR);
		
		score_size = read32(fp, BE_ENCODING);
		header_size = read32(fp, BE_ENCODING);
		frame_count = read32(fp, BE_ENCODING);
	}
	else if (header_size < 0) {
		abort_on_error("Invalid score header format");
	}
	
	/* unknown */ read16(fp, BE_ENCODING);
	int frame_size = read16(fp, BE_ENCODING);
	int channel_count = read16(fp, BE_ENCODING);
	/* unknown */ read16(fp, BE_ENCODING);
	
	if (header_size) {
		fseek(fp, header_size-20, SEEK_CUR);
		score_bytes_read = header_size;
	}
	// Tests the encoded data to check the frame count
	{
		long offset = ftell(fp);
		uint32_t frame_count_2 = 0;
		int i;
		for (i=0; score_bytes_read < score_size; i++) {
			
			int total_delta_size = read16(fp, BE_ENCODING);
			int delta_size;
			// int delta_offset;
			
			score_bytes_read += total_delta_size;
			total_delta_size -= 2;
			
			while (total_delta_size > 0) {
				delta_size = read16(fp, BE_ENCODING);
				/*delta_offset = */ read16(fp, BE_ENCODING);
				fseek(fp, delta_size, SEEK_CUR);
				total_delta_size -= 2+2+delta_size;
			}
			frame_count_2++;
		}
		fseek(fp, offset, SEEK_SET);
		score_bytes_read = header_size;
		
		if (frame_count < frame_count_2) {
			frame_count = frame_count_2;
			warning("Inferring frame count from delta decoded data");
		}
	}
	
	uint16_t *frame = calloc(frame_size/sizeof(uint16_t) * channel_count, sizeof(uint16_t));
	if (!frame) abort_on_error("Failed to allocate memory for the frame.");
	
	score->frame_count = frame_count;
	score->channel_count = channel_count -= 2;
	
	score->mainchannel = calloc(frame_count, FRAME_SIZE);
	score->palette = calloc(frame_count, FRAME_SIZE);
	score->frames = calloc(frame_count * channel_count, FRAME_SIZE);
	
	if (!score->mainchannel || !score->palette || !score->frames)
		abort_on_error("Failed to allocate memory for the score.");
	
	int i;
	for (i=0; score_bytes_read < score_size; i++) {
		
		int total_delta_size = read16(fp, BE_ENCODING);
		int delta_size;
		int delta_offset;
		
		score_bytes_read += total_delta_size;
		total_delta_size -= 2;
		
		while (total_delta_size > 0) {
			delta_size = read16(fp, BE_ENCODING);
			delta_offset = read16(fp, BE_ENCODING);
						
			int j;
			for (j=0; j<delta_size/2; j++) // Use fread instead?
				frame[delta_offset/2+j] = read16(fp, BE_ENCODING);
			
			total_delta_size -= 2+2+delta_size;
		}
		
		#define FRAME_INT_COUNT (frame_size/sizeof(uint16_t)) // The number of indexes on frame[]
		
		memcpy(score->mainchannel + i, frame                , FRAME_SIZE);
		memcpy(score->palette     + i, frame+FRAME_INT_COUNT, FRAME_SIZE);
		int k;
		for (k=0; k<channel_count; k++)
			memcpy(
				score->frames + i * channel_count + k,
				frame+(2+k)*FRAME_INT_COUNT,
				FRAME_SIZE
			);
	}
		
	free(frame);
	return 0;
}

void free_score(score_t *score) {
	if (!score) return;
	if (score->mainchannel)
		free(score->mainchannel);
	if (score->palette)
		free(score->palette);
	if (score->frames)
		free(score->frames);
	memset(score, 0, sizeof(score_t));
}

