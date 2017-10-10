/*
	D4Player
	* main.c
*/

#include "main.h"
#include "common.h"
#include "director/movie.h"
#include "player/player.h"
#include "extract.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <allegro.h> // END_OF_MAIN()

list_t extract_list = EMPTY_LIST;
list_t extract_4cc = EMPTY_LIST;
list_t convert_list = EMPTY_LIST;
list_t print_list = EMPTY_LIST;
list_t print_4cc = EMPTY_LIST;
list_t list_list = EMPTY_LIST;
list_t list_4cc = EMPTY_LIST;

char *filename;
char *log_filename;
int flags;

int is_directory(const char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

int test_or_create_dir(char *filename) {
	if (!mkdir(filename, 0755))
		return 0; // Creation successfull
	
	if (errno == EEXIST && is_directory(filename))
		return 0; // File exists and is a directory
	
	char *msg = NULL;
	switch (errno) {
		case EACCES: msg = "Permission denied"; break;
		case EEXIST: msg = "Filename already exists"; break;
		#ifdef ELOOP
		case ELOOP : msg = "Symbolic link loop encountered during path resolution"; break;
		#endif
		case EMLINK: msg = "The link count of the parent directory would exceed the maximum allowed"; break;
		case ENAMETOOLONG: msg = "The length of the path exceeds the maximum allowed"; break;
		case ENOENT: msg = "A component of the path does not name an existing directory"; break;
		case ENOSPC: msg = "Not enough space to hold the new directory"; break;
		case ENOTDIR:msg = "A component of the path is not a directory"; break;
		case EROFS : msg = "The parent directory resides on a read-only file system"; break;
	}
	if (msg)
		error_f("Failed to create directory '%s': %s", filename, msg);
	else
		error_f("Failed to create directory '%s'", filename);
	return -1;
}

void create_temp_dir() {
	if (test_or_create_dir(temp_dir))
		exit(EXIT_FAILURE);
}

void create_output_dir() {
	if (test_or_create_dir(output_dir))
		exit(EXIT_FAILURE);
}

int parse_args(int argc, char *argv[]) {
	if (argc < 2) {
		error("Missing parameters");
		return 1;
	}
	
	filename = NULL;
	temp_dir = DEFAULT_TEMP_FOLDER;
	output_dir = DEFAULT_OUTPUT_FOLDER;
	flags = FLAG_PLAY;
	
	int i;
	for (i=1; i<argc; i++) {
		// COMMAND LINE: -h
		if (strcmp(argv[i], "-h") == 0) {
			PRINT_HELP(argv[0]);
			exit(EXIT_SUCCESS);
		}
		// COMMAND LINE: -np
		else if (strcmp(argv[i], "-np") == 0) {
			flags &= ~FLAG_PLAY;
		}
		// COMMAND LINE: -i
		else if (strcmp(argv[i], "-i") == 0) {
			flags |= FLAG_INFO;
			flags &= ~FLAG_PLAY;
		}
		// COMMAND LINE: -t
		else if (strcmp(argv[i], "-t") == 0) {
			i++;
			if (i == argc) {
				error("Missing parameter for -t");
				return 1;
			}
			temp_dir = argv[i];
		}
		// COMMAND LINE: -o
		else if (strcmp(argv[i], "-o") == 0) {
			i++;
			if (i == argc) {
				error("Missing parameter for -o");
				return 1;
			}
			output_dir = argv[i];
		}
		// COMMAND LINE: -log
		else if (strcmp(argv[i], "-log") == 0) {
			i++;
			if (i == argc) {
				error("Missing parameter for -log");
				return 1;
			}
			if (strcmp(argv[i], "stdout") == 0) {
				flags |= FLAG_LOG_STDOUT;
			}
			else if (strcmp(argv[i], "stderr") == 0) {
				flags |= FLAG_LOG_STDERR;
			}
			else {
				flags |= FLAG_LOG_FILE;
				log_filename = argv[i];
			}
		}
		// COMMAND LINE: -x
		else if (strcmp(argv[i], "-x") == 0) {
			i++;
			if (i == argc) {
				error("Missing parameter for -x");
				return 1;
			}
			flags &= ~FLAG_PLAY;
			if (strcmp(argv[i], "all") == 0) {
				flags |= FLAG_EXTRACT_ALL;
			}
			else if (strcmp(argv[i], "nojunk") == 0) {
				flags |= FLAG_EXTRACT_ALL | FLAG_EXTRACT_NOJUNK;
			}
			else {
				int index = atol(argv[i]);
				if (index)
					push(&extract_list, index);
				else if (strlen(argv[i]) == 4) {
					push(&extract_4cc, MKTAG(argv[i]));
				}
				else {
					error_f("Invalid parameter for -x: %s", argv[i]);
					return 1;
				}
			}
		}
		// COMMAND LINE: -c
		else if (strcmp(argv[i], "-c") == 0) {
			i++;
			if (i == argc) {
				error("Missing parameter for -c");
				return 1;
			}
			flags |= FLAG_CONVERT;
			flags &= ~FLAG_PLAY;
			if (strcmp(argv[i], "all") == 0)
				flags |= FLAG_CONVERT_ALL;
			else if (strcmp(argv[i], "sound") == 0)
				flags |= FLAG_CONVERT_SOUND;
			else if (strcmp(argv[i], "image") == 0)
				flags |= FLAG_CONVERT_IMAGE;
			else if (strcmp(argv[i], "score") == 0)
				flags |= FLAG_CONVERT_SCORE;
			else if (strcmp(argv[i], "script") == 0)
				flags |= FLAG_CONVERT_SCRIPT;
			else {
				int index = atol(argv[i]);
				if (!index) {
					error_f("Invalid parameter for -c: %s", argv[i]);
					return 1;
				}
				push(&convert_list, index);
			}
		}
		// COMMAND LINE: -p
		else if (strcmp(argv[i], "-p") == 0) {
			flags &= ~FLAG_PLAY;
			i++;
			if (i == argc) {
				error("Missing parameter for -p");
				return 1;
			}
			int index = atol(argv[i]);
			if (index)
				push(&print_list, index);
			else if (strlen(argv[i]) == 4) {
				push(&print_4cc, MKTAG(argv[i]));
			}
			else {
				error_f("Invalid parameter for -p: %s", argv[i]);
				return 1;
			}
		}
		// COMMAND LINE: -l
		else if (strcmp(argv[i], "-l") == 0) {
			flags &= ~FLAG_PLAY;
			i++;
			if (i == argc) {
				error("Missing parameter for -l");
				return 1;
			}
			int index = atol(argv[i]);
			if (index)
				push(&list_list, index);
			else if (strlen(argv[i]) == 4) {
				push(&list_4cc, MKTAG(argv[i]));
			}
			else {
				error_f("Invalid parameter for -l: %s", argv[i]);
				return 1;
			}
		}
		else {
			if (!filename) {
				filename = argv[i];
			}
			else {
				error_f("Unrecognized option: %s", argv[i]);
				return 1;
			}
		}
	}
	if (!filename) {
		error("Missing parameter: filename");
		return 1;
	}
	return 0;
}

void print_info(movie_t *movie, int32_t index) {
	if (index < 0 || index >= movie->mmap.size) {
		error_f("Index %d is out of bounds", index);
		return;
	}
	switch(movie->mmap.entries[index].type) {
		case MKTAG2('i','m','a','p'):
			print_imap(&movie->imap);
			break;
		case MKTAG2('m','m','a','p'):
			print_mmap_header(&movie->mmap);
			break;
		case MKTAG2('K','E','Y','*'):
			print_keys_header(&movie->keys);
			break;
		case MKTAG2('C','A','S','*'):
			print_castlist_header(&movie->castlist);
			break;
		case MKTAG2('V','W','C','F'):
			print_movieinfo(&movie->info);
			break;
		default:
			warning_f("Cannot print info about %c%c%c%c blocks",
				PRNT_TAG(movie->mmap.entries[index].type)
			);
			break;
	}
}

void print_entries(movie_t *movie, int32_t index) {
	if (index < 0 || index >= movie->mmap.size) {
		error_f("Index %d is out of bounds", index);
		return;
	}
	switch(movie->mmap.entries[index].type) {
		case MKTAG2('m','m','a','p'):
			print_mmap_entries(&movie->mmap);
			break;
		case MKTAG2('K','E','Y','*'):
			list_keys(&movie->keys);
			break;
		case MKTAG2('C','A','S','*'):
			print_castlist2(movie);
			break;
		default:
			warning_f("Cannot list entries for %c%c%c%c blocks",
				PRNT_TAG(movie->mmap.entries[index].type)
			);
			break;
	}
}

void rip_index(movie_t *movie, int32_t index) {
	if (seek_movie(movie, index)) {
		error_f("Index %d is out of bounds", index);
		return;
	}
	long size = mmap_get_length(&movie->mmap, index) + 8;
	char filename[255];
	sprintf(filename, "%s/%04d.raw", output_dir, index);
	FILE *file = fopen(filename, "wb");
	rip(movie->file, file, size);
	fclose(file);	
}

int main(int argc, char *argv[]) {
	set_log_file(stderr);
	
	if (parse_args(argc, argv)) {
		fprintf(stderr, "Try '%s -h' for help.\n\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	if (flags & FLAG_LOG_STDOUT)
		set_log_file(stdout);
	if (flags & FLAG_LOG_STDOUT)
		set_log_file(stdout);
	if (flags & FLAG_LOG_FILE) {
		FILE *log_file = fopen(log_filename, "a");
		if (!log_file) {
			error_f("Could not open file for writing: %s", log_filename);
			exit(EXIT_FAILURE);
		}
		set_log_file(log_file);
	}
	
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		error_f("Could not open file for reading: %s", filename);
		exit(EXIT_FAILURE);
	}
	
	movie_t movie;
	if (read_movie(&movie, fp)) {
		exit(EXIT_FAILURE);
	}
	
	if (flags & FLAG_INFO) {
		warning("Option: -i - Not implemented");
	}
	
	// Extract
	if (flags & FLAG_EXTRACT_ALL) {
		create_output_dir();
		int index;
		for (index = 1; index < movie.mmap.size; index++) {
			// Skip junk/free entries
			if (flags & FLAG_EXTRACT_NOJUNK) {
				if (movie.mmap.entries[index].type == MKTAG("junk")
				||  movie.mmap.entries[index].type == MKTAG("free"))
				{
					continue;
				}
			}
			rip_index(&movie, index);
		}
	}
	else {
		if (extract_list.size > 0 || extract_4cc.size > 0)
			create_output_dir();
		int32_t value;
		while ((value = unshift(&extract_list)) >= 0) {
			rip_index(&movie, value);
		}
		while ((value = unshift(&extract_4cc)) >= 0) {
			char tag[4];
			UNMKTAG(tag,value);
			int index = 0;
			while ((index = mmap_search(&movie.mmap, tag, ++index)) > 0) {
				rip_index(&movie, index);
			}
		}
	}
	// Convert
	if (flags & FLAG_CONVERT) {
		create_temp_dir();
		create_output_dir();
		
		if (flags & FLAG_CONVERT_SOUND)
			extract_sound(&movie);
		if (flags & FLAG_CONVERT_IMAGE)
			extract_image(&movie, 0);
		if (flags & FLAG_CONVERT_SCORE)
			extract_score(&movie);
		if (flags & FLAG_CONVERT_SCRIPT)
			extract_script(&movie);
	}
	
	int32_t value;
	// Print entries
	while ((value = unshift(&print_list)) >= 0) {
		print_info(&movie, value);
	}
	while ((value = unshift(&print_4cc)) >= 0) {
		char tag[4];
		UNMKTAG(tag,value);
		int index = mmap_search(&movie.mmap, tag, 0);
		if (index < 0) {
			error_f("No %c%c%c%c blocks found on mmap", PRNT_TAG(value));
			continue;
		}
		print_info(&movie, index);
	}
	// List entries
	while ((value = unshift(&list_list)) >= 0) {
		print_entries(&movie, value);
	}
	while ((value = unshift(&list_4cc)) >= 0) {
		char tag[4];
		UNMKTAG(tag,value);
		int index = mmap_search(&movie.mmap, tag, 0);
		if (index < 0) {
			error_f("No %c%c%c%c blocks found on mmap", PRNT_TAG(value));
			continue;
		}
		print_entries(&movie, index);
	}
	
	// Play movie
	if (flags & FLAG_PLAY) {
		if (movie.score.frame_count == 0) {
			warning("Nothing to play");
		}
		else {
			create_temp_dir();
			create_output_dir();
			player(&movie);
		}
	}
	
	free_movie(&movie);
	
	return 0;
}
END_OF_MAIN()
