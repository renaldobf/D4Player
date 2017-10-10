/*
	D4Player
	* wave.h
	
	RIFF WAVE wrapper
*/

#ifndef _WAVE_H
#define _WAVE_H

#include <stdio.h>

// Define datatypes
typedef unsigned char BYTE;

// RIFF header definition
struct RIFF_header_t {
	BYTE ChunkID[4];
	BYTE ChunkSize[4];
	BYTE Format[4];
};

// Format subchunk definition
struct fmt_subchunk_t {
	BYTE Subchunk1ID[4];
	BYTE Subchunk1Size[4];
	BYTE AudioFormat[2];
	BYTE NumChannels[2];
	BYTE SampleRate[4];
	BYTE ByteRate[4];
	BYTE BlockAlign[2];
	BYTE BitsPerSample[2];
};

// Data subchunk definition
struct data_subchunk_t {
	BYTE Subchunk2ID[4];
	BYTE Subchunk2Size[4];
};

// Converts an integer to little endian format
void int_to_little_endian (unsigned number, BYTE *array, int size);
// Converts from little endian format to integer
unsigned little_endian_to_int (BYTE *array, int size);
// Stores the string in a byte array
void string_to_array (BYTE *array, int size, const char value[]);
// Input is expected to be in big endian (AIFF-like), but need to output to little endian (WAVE style)
void swap_bytes(unsigned char *p, long size);
// Create the RIFF wave file
int write_wave (FILE *output, int channels, int sample_rate, int samples, int bps, FILE *input);

#endif
