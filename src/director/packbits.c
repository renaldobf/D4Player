/*
	D4Player
	* packbits.c
*/

#include "packbits.h"
#include "../common.h"
#include <stdio.h>
#include <stdlib.h>

int unpack_packbits_1(FILE *fp, FILE *fo, long size) {
	char b;
	long total_read = 0;
	long total_size = 0;
	while (total_read < size) {
		if (fread(&b,1,1,fp) < 1) return 0;
		total_read++;
		
		if (fwrite(&b,1,1,fo) < 1) return 0;
		total_size++;
	}
	return total_size;
}

int unpack_packbits_2(FILE *fp, FILE *fo, long size) {
	unsigned char b[4];
	long total_read = 0;
	long total_size = 0;
	while (total_read < size) {
		if (fread(b,1,4,fp) < 1) return 0;
		total_read+=4;
		
		int tmp = b[0];
		
		b[0] = b[1];
		b[1] = b[2];
		b[2] = b[3];
		b[3] = 255 - tmp;
		
		if (fwrite(&b,1,4,fo) < 1) return 0;
		total_size+=4;
	}
	return total_size;
}

int unpack_packbits_8(FILE *fp, FILE *fo, long size) {
	char b, c;
	int i;
	long total_read = 0;
	long total_size = 0;
	while (total_read < size) {
		if (fread(&b,1,1,fp) < 1) {
			return 0;
		}
		total_read++;
		
		if (b >= 0) {
			for (i=0; i<1+b;i++)  {
				if (fread(&c,1,1,fp) < 1) return 0;
				if (fwrite(&c,1,1,fo) < 1) return 0;
				total_read++;
				total_size++;
			}
		}
		else /* if (b >= -127) */ {
			if (fread(&c,1,1,fp) < 1) return 0;
			total_read++;
			for (i=0; i<1-b;i++) {
				if (fwrite(&c,1,1,fo) < 1) return 0;
				total_size++;
			}
		}
	}
	return total_size;
}

int unpack_packbits_24(FILE *fp, FILE *fo, long size, int width) {
	char *buffer = malloc(width*3);
	if (!buffer) {
		abort_on_error_f("Error alocating memory: %d bytes", width*3);
	}
	char b, c;
	long total_read = 0;
	long total_size = 0;
	int i, j = 0, k;
	while (total_read < size) {
		if (fread(&b,1,1,fp) < 1) return 0;
		total_read++;
		
		if (b >= 0) {
			for (i=0; i<1+b;i++)  {
				if (fread(&c,1,1,fp) < 1) return 0;
				total_read++;
				buffer[j++] = c;
			}
		}
		else /*if (b >= -127)*/ {
			if (fread(&c,1,1,fp) < 1) return 0;
			total_read++;
			for (i=0; i<1-b;i++) {
				buffer[j++] = c;
			}
		}
		if (j == width*3) {
			for (k=0; k<width; k++) {
				if (fwrite(buffer+k+width*0,1,1,fo) < 1) return 0;
				if (fwrite(buffer+k+width*1,1,1,fo) < 1) return 0;
				if (fwrite(buffer+k+width*2,1,1,fo) < 1) return 0;
				total_size+=3;
			}
			j=0;
		}
	}
	free(buffer);
	return total_size;
}

int unpack_packbits_32(FILE *fp, FILE *fo, long size, int width) {
	char *buffer = malloc(width*4);
	if (!buffer) {
		abort_on_error_f("Error alocating memory: %d bytes", width*4);
	}
	char b, c;
	long total_read = 0;
	long total_size = 0;
	int i, j = 0, k;
	while (total_read < size) {
		if (fread(&b,1,1,fp) < 1) return 0;
		total_read++;
		if (b >= 0) {
			for (i=0; i<1+b;i++)  {
				if (fread(&c,1,1,fp) < 1) return 0;
				total_read++;
				buffer[j++] = c;
			}
		}
		else /*if (b >= -127)*/ {
			if (fread(&c,1,1,fp) < 1) return 0;
			total_read++;
			for (i=0; i<1-b;i++) {
				buffer[j++] = c;
			}
		}
		if (j == width*4) {
			for (k=0; k<width; k++) {
				if (fwrite(buffer+k+width*1,1,1,fo) < 1) return 0;
				if (fwrite(buffer+k+width*2,1,1,fo) < 1) return 0;
				if (fwrite(buffer+k+width*3,1,1,fo) < 1) return 0;
				c = 255 - (unsigned char)buffer[k];
				if (fwrite(&c,1,1,fo) < 1) return 0;
				total_size+=4;
			}
			j=0;
		}
	}
	free(buffer);
	return total_size;
}
