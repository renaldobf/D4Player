/*
	D4Player
	* movie.c
*/

#include "movie.h"

#include "../common.h"
#include "rifxhead.h"
#include "imap.h"
#include "mmap.h"
#include "movieinf.h"
#include "keys.h"
#include "castlist.h"
#include "score.h"

#include "cast.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

int read_movie(movie_t *movie, FILE *fp) {
	movie->file = fp;
	movie->offset = ftell(fp);
	
	// Read header	
	movie->encoding = read_rifx_header(&movie->file_header, fp);
	if (movie->encoding < 0)
		return 1;
	// Read imap
	if (read_imap(&movie->imap, fp, movie->encoding))
		return 1;
	// Read mmap
	fseek(fp, movie->offset + movie->imap.offset, SEEK_SET);
	
	if (read_mmap(&movie->mmap, fp, movie->encoding))
		return 1;
	// Read movie info
	if (seek_movie_search(movie, "VWCF", 0)) {
		warning("VWCF block not present on the mmap.");
		memset(&movie->info, 0, sizeof(movieinfo_t));
	}
	else if (read_movieinfo(&movie->info, fp, movie->encoding)) {
		goto clean_mmap;
	}
	// Read keys
	if (seek_movie_search(movie, "KEY*", 0)) {
		error("KEY* block not present on the mmap.");
		goto clean_mmap;
	}
	if (read_keys(&movie->keys, fp, movie->encoding)) {
		goto clean_mmap;
	}
	// Read cast
	if (seek_movie_search(movie, "CAS*", 0)) {
		error("CAS* block not present on the mmap.");
		goto clean_keys;
	}
	if (read_castlist(&movie->castlist, fp, movie->encoding)) {
		goto clean_keys;
	}
	// Read score
	if (seek_movie_search(movie, "VWSC", 0)) {
		warning("Score block not present on the mmap.");
		memset(&movie->score, 0, sizeof(score_t));
	}
	else if (read_score(&movie->score, fp, movie->encoding)) {
		goto clean_castlist;
	}
	
	// Return success
	return 0;
	
	/* Cleaning up on error */
clean_castlist:
	free_castlist(&movie->castlist);
clean_keys:
	free_keys(&movie->keys);
clean_mmap:
	free_mmap(&movie->mmap);
	
	// Return fail
	return 1;
}

void free_movie(movie_t *movie) {
	if (!movie) return;
	free_keys(&movie->keys);
	free_mmap(&movie->mmap);
	free_castlist(&movie->castlist);
	free_score(&movie->score);
	memset(movie, 0, sizeof(movie_t));
}

int seek_movie(movie_t *movie, int32_t index) {
	int32_t offset = mmap_get_offset(&movie->mmap, index);
	if (offset < 0)
		return 1;
	fseek(movie->file, movie->offset + offset, SEEK_SET);
	return 0;
}

int seek_movie_search(movie_t *movie, const char *type, int32_t start) {
	int32_t index = mmap_search(&movie->mmap, type, start);
	if (index < 0) {
		return 1;
	}
	int32_t offset = mmap_get_offset(&movie->mmap, index);
	fseek(movie->file, movie->offset + offset, SEEK_SET);
	return 0;
}

void print_castlist2(movie_t *movie) {
	if (!movie || !movie->castlist.entries) return;
	printf(" cast | index | name\n");
	printf("--------------------------------------------------\n");
	int32_t i;
	for (i=0; i<movie->castlist.size; i++) {
		if (movie->castlist.entries[i]) {
			cast_t cast;
			seek_movie(movie, movie->castlist.entries[i]);
			read_cast(&cast, movie->file);
			if (cast.name[0])
				printf(" %4d | %5d | %s\n", movie->info.castlist_start+i, movie->castlist.entries[i], cast.name);
			else
				printf(" %4d | %5d |\n", movie->info.castlist_start+i, movie->castlist.entries[i]);
		}
		else
			printf(" %4d |       |\n", movie->info.castlist_start+i);
	}
}
