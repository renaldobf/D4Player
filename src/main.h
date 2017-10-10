#ifndef _MAIN_H
#define _MAIN_H

#define APPLICATION_NAME "D4Player"
#define APPLICATION_VERSION "0.0"

#define DEFAULT_TEMP_FOLDER "tmp"
#define DEFAULT_OUTPUT_FOLDER "output"

#define PRINT_HELP(a) printf(\
	APPLICATION_NAME " - v" APPLICATION_VERSION "\n"\
	"Usage: %s [OPTION]... FILE\n"\
	"\n"\
	"   -h            Prints this help and exits.\n"\
	"   -np           Reads the file and exits.\n"\
	"   -i            Prints information about the file.\n"\
	"   -t TEMPDIR    Uses TEMPDIR as temporary directory.\n"\
	"   -o OUTDIR     Sets OUTDIR as the directory for extraction.\n"\
	"   -log VALUE    Sets the destination for the logs.\n"\
	"                 \"stdout\" - logs to standart output\n"\
	"                 \"stderr\" - logs to standart error output\n"\
	"                 [FILE]   - logs to FILE in append mode\n"\
	"   -log VALUE    Sets the destination for the logs.\n"\
	"\n"\
	"   -x VALUE      Extracts the raw contents of the block.\n"\
	"                 \"all\"    - extracts all blocks\n"\
	"                 \"nojunk\" - extracts all blocks skipping junk/free entries\n"\
	"                 [BLOCK]  - extracts all blocks with BLOCK header\n"\
	"                 [INDEX]  - extracts the block on INDEX position\n"\
	"\n"\
	"   -c VALUE      Tries to convert elements.\n"\
	"                 \"all\"   - converts all supported blocks\n"\
	"                 \"sound\" - converts all sounds in file\n"\
	"                 \"image\" - converts all images in file\n"\
	"                 \"score\" - converts the score\n"\
	"                 \"script\"- converts all scripts in file\n"\
/*	"                 [INDEX] - converts the block on INDEX position\n"*/\
	"\n"\
	"   -p VALUE      Prints information about the block\n"\
	"                 [INDEX] - prints the block on INDEX position\n"\
	"                 [BLOCK] - prints the first block with BLOCK header\n"\
	"\n"\
	"   -l VALUE      Lists the entries on the block\n"\
	"                 [INDEX] - lists the block on INDEX position\n"\
	"                 [BLOCK] - lists the first block with BLOCK header\n"\
	"\n", (a))

#define FLAG_PLAY           0x0001
#define FLAG_INFO           0x0002
#define FLAG_EXTRACT_ALL    0x0004
#define FLAG_EXTRACT_NOJUNK 0x0008
#define FLAG_CONVERT        0x0010
#define FLAG_CONVERT_SOUND  0x0020
#define FLAG_CONVERT_IMAGE  0x0040
#define FLAG_CONVERT_SCORE  0x0080
#define FLAG_CONVERT_SCRIPT 0x0100
#define FLAG_CONVERT_ALL (FLAG_CONVERT_SOUND|FLAG_CONVERT_IMAGE|FLAG_CONVERT_SCORE|FLAG_CONVERT_SCRIPT)
#define FLAG_LOG_STDOUT     0x0200
#define FLAG_LOG_STDERR     0x0400
#define FLAG_LOG_FILE       0x0800

#endif
