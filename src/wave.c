/*
	D4Player
	* wave.c
*/

#include "wave.h"
#include "common.h"

void int_to_little_endian (unsigned number, BYTE *array, int size) {
	int i;
	for (i=0; i<size; i++) {
		array[i] = number % 0x100;
		number >>= 8;
	}
}

unsigned little_endian_to_int (BYTE *array, int size) {
	int i;
	unsigned number = 0;
	for (i=0; i<size; i++) {
		number <<= 8;
		number += array[i];
	}
	return number;
}

void string_to_array (BYTE *array, int size, const char value[]) {
	int i;
	for (i=0; i<size; i++) {
		array[i] = (BYTE) value[i];
	}	
}

void swap_bytes(unsigned char *p, long size) {
	long i;
	unsigned char c;
	for (i=0; i<size; i+=2) {
		c = p[i];
		p[i] = p[i+1];
		p[i+1] = c;
	}
}

int write_wave (FILE *output, int channels, int sample_rate, int samples, int bps, FILE *input) {
	struct RIFF_header_t header;
	struct fmt_subchunk_t fmt;
	struct data_subchunk_t data;
	
	// Header
	string_to_array (header.ChunkID, 4, "RIFF");
	int_to_little_endian (sizeof(struct RIFF_header_t) - 8
	                     +sizeof(struct fmt_subchunk_t)
	                     +sizeof(struct data_subchunk_t)
	                     +samples * channels * bps / 8,
	                     header.ChunkSize, 4);
	string_to_array (header.Format,  4, "WAVE");
	// FMT datachunk
	string_to_array (fmt.Subchunk1ID, 4, "fmt ");
	int_to_little_endian (sizeof(struct fmt_subchunk_t) - 8,
	                                   fmt.Subchunk1Size, 4);
	int_to_little_endian (          1, fmt.AudioFormat,   2);
	int_to_little_endian (   channels, fmt.NumChannels,   2);
	int_to_little_endian (sample_rate, fmt.SampleRate,    4);
	int_to_little_endian (sample_rate * channels * bps/8,
	                                   fmt.ByteRate,      4);
	int_to_little_endian ( channels * bps/8,
	                                   fmt.BlockAlign,    2);
	int_to_little_endian (        bps, fmt.BitsPerSample, 2);
	// DATA datachunk
	string_to_array(data.Subchunk2ID,   4, "data");
	int_to_little_endian (samples * channels * bps/8,
	                                  data.Subchunk2Size, 4);

	if (fwrite(&header, sizeof(struct RIFF_header_t)  , 1, output) <= 0
	||  fwrite(&fmt   , sizeof(struct fmt_subchunk_t) , 1, output) <= 0
	||  fwrite(&data  , sizeof(struct data_subchunk_t), 1, output) <= 0 )
	{
		error("Error while writing wave file: could not write WAVE header.");
		return 1;
	}
	
	unsigned long length = channels * samples * bps / 8;
	unsigned char buffer[COPY_BUFFER_SIZE];
	unsigned long bytes_read;
	while (length) {
		if (length < COPY_BUFFER_SIZE)
			bytes_read = fread(buffer,1,length,input);
		else
			bytes_read = fread(buffer,1,COPY_BUFFER_SIZE,input);
		if (bytes_read <= 0) break;
		if (bps > 8) swap_bytes(buffer, bytes_read);
		if (fwrite(buffer,1,bytes_read,output) <= 0) return 1;
		length -= bytes_read;
	}
	if (length != 0) {
		error("Error while writing wave file: not matching filesizes.");
		return 1;
	}
	return 0;
}
