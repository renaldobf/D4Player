/*
	D4Player
	* sound.h
	 
	== snd  ==
	This block stores uncompressed audio on the archive.
	There are aparently 2 different types of snd blocks and 2 subtypes for type-2 block.
	The type is defined on the first 16-bit integer.
	The subtypes have no apparent distinction on their headers.
	
	The only way I found to compare then is to look at the channel count on the file:
	* If it is 1 or 2, it's the regular subtype-1 sound block.
	* Otherwise, it's a subtype-2 block and the value read is actually the sample count.
	
	The subtype-2 is a single channel unsigned 8-bit PCM.
	Sometimes, the sample rate is set as 0x7FFF. On these files, I'm using 22254 Hz as a fallback.
	22254 Hz is the Apple Macintosh audio hardware native sample rate.
	
	There are 2 encodings:
	 - unsigned 8-bit PCM
	 - signed 16-bit PCM (Big Endian)
	
*/

#ifndef _SOUND_H
#define _SOUND_H

#include <stdio.h>
#include <stdint.h>

#define FALLBACK_SAMPLE_RATE 22254
#define FALLBACK_BPS 8
#define FALLBACK_CHANNELS 1

typedef struct {
	unsigned char header[4];
	uint32_t filesize;
} snd_header_t;

#define SND_HEADER_SIZE 8

typedef struct {
	snd_header_t header;
	int type, channels, sample_rate, samples, bps;
} sount_t;

int read_sound(sount_t *snd, FILE *fp);
void print_sound(sount_t *snd);

#endif
