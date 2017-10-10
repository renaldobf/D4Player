/*
	D4Player
	* sound.c
*/

#include "sound.h"
#include "../common.h"

#include <stdio.h>

#define read16(x) read16((x), BE_ENCODING)
#define read32(x) read32((x), BE_ENCODING)

int read_snd_header(sount_t *snd, FILE *fp) {
	if (fread(&snd->header, SND_HEADER_SIZE, 1, fp) < 1) return 1;
	I32_FROM_BE(snd->header.filesize);
	return 0;
}

int read_snd_2_1(sount_t *snd, FILE *fp) {
	read16(fp);//2
	read16(fp);//0
	read32(fp);//98385
	read16(fp);//0
	read32(fp);//14
	read32(fp);//0
	snd->channels = read32(fp); // channels
	snd->sample_rate = read16(fp); // sample rate
	read16(fp);//??
	read32(fp);//0
	snd->samples = read32(fp); // samples
	read16(fp);//??
	
	if (snd->samples == 0)
		snd->samples = read32(fp); // samples
	else
		read32(fp);
	
	read16(fp);//??
	read16(fp);//??
	read16(fp);//??
	read32(fp);//0
	read32(fp);//0
	read32(fp);//0
	read32(fp);//0
	snd->bps = read16(fp); // bps
	read32(fp);//0
	read32(fp);//0
	read32(fp);//0
	read16(fp);//0
	
	return 0;
}

int read_snd_2_2(sount_t *snd, FILE *fp) {
	read16(fp);//2
	read16(fp);//0
	read32(fp);//98385
	read16(fp);//0
	read32(fp);//14
	read32(fp);//0
	snd->samples = read32(fp); // samples
	snd->sample_rate = read16(fp); // sample rate
	if (snd->sample_rate == 0x7FFF)
		snd->sample_rate = FALLBACK_SAMPLE_RATE;
	read16(fp);//??
	read32(fp);//0
	if (snd->samples == 0)
		snd->samples = read32(fp); //samples
	else
		read32(fp);
	read16(fp);//60
	snd->channels = FALLBACK_CHANNELS;
	snd->bps = FALLBACK_BPS;
	
	return 0;
}

int read_snd_1(sount_t *snd, FILE *fp) {
	read16(fp);//1
	read16(fp);//1
	read16(fp);//5
	read16(fp);//0
	read16(fp);//160
	read32(fp);//98385
	read32(fp);//0
	read16(fp);//20
	read32(fp);//0
	snd->samples = read32(fp); // samples
	snd->sample_rate = read16(fp); // sample rate
	read16(fp);//0
	read32(fp);// samples-2?
	read32(fp);// samples-1?
	read16(fp);//60
	snd->channels = FALLBACK_CHANNELS;
	snd->bps = FALLBACK_BPS;
	
	return 0;
}

int snd_detect_type(FILE *fp) {
	long offset = ftell(fp);
	int type = read16(fp);
	fseek(fp,16,SEEK_CUR);
	int num = read32(fp);
	fseek(fp,offset,SEEK_SET);
	
	if (type == 1)
		return 1;
	else if (type == 2) {
		if (num > 2)
			return 3;
		return 2;
	}
	else
		return -1; // Uknown
}

int read_sound(sount_t *snd, FILE *fp) {
	if (read_snd_header(snd, fp)) return 1;
	snd->type = snd_detect_type(fp);
	switch(snd->type) {
		case 1:
			if(read_snd_1(snd, fp)) return 1;
			break;
		case 2:
			if(read_snd_2_1(snd, fp)) return 1;
			break;
		case 3:
			if(read_snd_2_2(snd, fp)) return 1;
			break;
		default:
			warning("Unknown sound type");
			return 1;
			break;
	}
	
	int offset = 0;
	switch (snd->type) {
		case 1: offset = 42; break;
		case 2: offset = 78; break;
		case 3: offset = 36; break;
	}
	if (snd->samples * snd->channels * snd->bps/8 + offset < snd->header.filesize)
		warning("Sample size smaller than block size!");
	if (snd->samples * snd->channels * snd->bps/8 + offset > snd->header.filesize) {
		error("Sample size bigger than block size!");
		return 1;
	}
	
	return 0;
}

void print_sound(sount_t *snd) {
	printf("Type %d : %d samples, %d channels, %d sample rate, %d bits\n",
		snd->type,
		snd->samples,
		snd->channels,
		snd->sample_rate,
		snd->bps);
}
