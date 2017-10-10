/*
	D4Player
	* extract.h
*/

#ifndef _EXTRACT_H
#define _EXTRACT_H

#include "director/movie.h"

void extract_image(movie_t *movie, int32_t clut_index);
void extract_sound(movie_t *movie);
void extract_score(movie_t *movie);
void extract_script(movie_t *movie);
void rip(FILE *fp, FILE *output, long length);
void rip_mac(FILE *fp, FILE *output, long length);

#endif
