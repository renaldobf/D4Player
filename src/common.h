/*
	D4Player
	* common.h
*/

#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>
#include <stdint.h>

// Buffer size for file copying
#define COPY_BUFFER_SIZE 4096

#if defined(BIG_ENDIAN_SYSTEM) && defined(LITTLE_ENDIAN_SYSTEM)
	#error "Both big-endian and little-endian flags were set"
#endif

#if !defined(BIG_ENDIAN_SYSTEM) && !defined(LITTLE_ENDIAN_SYSTEM)
	#define LITTLE_ENDIAN_SYSTEM
#endif

#define SWAP_32(x) ( x = \
	((uint32_t)(x) & 0xFF) << 24 |\
	((uint32_t)(x) & 0xFF00) << 8 |\
	((uint32_t)(x) & 0xFF0000) >> 8 |\
	((uint32_t)(x) & 0xFF000000) >> 24 )

#define SWAP_16(x) ( x = \
	((uint16_t)(x) & 0xFF) << 8 |\
	((uint16_t)(x) & 0xFF00) >> 8)

#ifdef BIG_ENDIAN_SYSTEM
	#define I32_FROM_LE(x) SWAP_32(x)
	#define I16_FROM_LE(x) SWAP_16(x)
	#define I32_FROM_BE(x) ((x)=(uint32_t)(x))
	#define I16_FROM_BE(x) ((x)=(uint16_t)(x))
#else
	#define I32_FROM_LE(x) ((x)=(uint32_t)(x))
	#define I16_FROM_LE(x) ((x)=(uint16_t)(x))
	#define I32_FROM_BE(x) SWAP_32(x)
	#define I16_FROM_BE(x) SWAP_16(x)
#endif // BIG_ENDIAN_SYSTEM

#define BE_ENCODING 0
#define LE_ENCODING 1
#define CONVERT_32(e,x) ((e)?I32_FROM_LE(x):I32_FROM_BE(x))
#define CONVERT_16(e,x) ((e)?I16_FROM_LE(x):I16_FROM_BE(x))

#define MKTAG(x) ((uint32_t) (\
	((uint32_t)(x)[0] << 24)|\
	((uint32_t)(x)[1] << 16)|\
	((uint32_t)(x)[2] <<  8)|\
	((uint32_t)(x)[3] <<  0)))

#define MKTAG2(a,b,c,d) ((uint32_t) (\
	((uint32_t)(a) << 24)|\
	((uint32_t)(b) << 16)|\
	((uint32_t)(c) <<  8)|\
	((uint32_t)(d) <<  0)))

#define PRNT_TAG(x) \
	(char)(((uint32_t)(x) >> 24) & 0xFF),\
	(char)(((uint32_t)(x) >> 16) & 0xFF),\
	(char)(((uint32_t)(x) >>  8) & 0xFF),\
	(char)(((uint32_t)(x) >>  0) & 0xFF)

#define UNMKTAG(t,x) \
	(t)[0] = (((uint32_t)(x) >> 24) & 0xFF),\
	(t)[1] = (((uint32_t)(x) >> 16) & 0xFF),\
	(t)[2] = (((uint32_t)(x) >>  8) & 0xFF),\
	(t)[3] = (((uint32_t)(x) >>  0) & 0xFF)

struct list_node_t {
	struct list_node_t *next;
	struct list_node_t *prev;
	int32_t value;
};

typedef struct {
	struct list_node_t *head;
	struct list_node_t *tail;
	int size;
} list_t;

#define EMPTY_LIST {NULL,NULL,0}

extern char *temp_dir;
extern char *output_dir;

void set_log_file(FILE *fp);

void debug(const char *message);
void debug_f(const char *format, ...);
void warning(const char *message);
void warning_f(const char *format, ...);
void error(const char *message);
void error_f(const char *format, ...);
void abort_on_error(const char *message);
void abort_on_error_f(const char *format, ...);
void exit_on_error(const char *message, int status);

uint8_t read8(FILE *fp);
uint16_t read16(FILE *fp, short encoding);
uint32_t read32(FILE *fp, short encoding);

void push(list_t *list, int32_t value);
int32_t pop(list_t *list);
int32_t unshift(list_t *list);

#endif
