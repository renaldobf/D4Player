/*
	D4Player
	* movie.h
*/

#ifndef _MOVIE_H
#define _MOVIE_H

#include "rifxhead.h"
#include "imap.h"
#include "mmap.h"
#include "movieinf.h"
#include "keys.h"
#include "castlist.h"
#include "score.h"

#include <stdio.h>
#include <stdint.h>

typedef struct {
	FILE *file;
	long offset;
	rifx_header_t file_header;
	movieinfo_t info;
	imap_t imap;
	mmap_t mmap;
	keys_t keys;
	castlist_t castlist;
	score_t score;
	short encoding;
} movie_t;

int read_movie(movie_t *movie, FILE *fp);
void free_movie(movie_t *movie);
int seek_movie(movie_t *movie, int32_t index);
int seek_movie_search(movie_t *movie, const char *type, int32_t start);
void print_castlist2(movie_t *movie);

#endif
