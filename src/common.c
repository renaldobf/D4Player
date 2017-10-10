/*
	D4Player
	* common.c
*/

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

FILE *log_file;
char *temp_dir;
char *output_dir;

void set_log_file(FILE *fp) {
	log_file = fp;
}

void debug(const char *message) {
	#ifdef DEBUG
	fprintf(log_file,"Debug: %s\n",message);
	#endif
}
void debug_f(const char *format, ...) {
	#ifdef DEBUG
	fprintf(log_file,"Debug: ");
    va_list argptr;
    va_start(argptr, format);
    vfprintf(log_file, format, argptr);
    va_end(argptr);
    fprintf(log_file,"\n");
    #endif
}

void warning(const char *message) {
	fprintf(log_file,"Warning: %s\n",message);
}
void warning_f(const char *format, ...) {
	fprintf(log_file,"Warning: ");
    va_list argptr;
    va_start(argptr, format);
    vfprintf(log_file, format, argptr);
    va_end(argptr);
    fprintf(log_file,"\n");
}


void error(const char *message) {
	fprintf(log_file,"Error: %s\n",message);
}
void error_f(const char *format, ...) {
	fprintf(log_file,"Error: ");
    va_list argptr;
    va_start(argptr, format);
    vfprintf(log_file, format, argptr);
    va_end(argptr);
    fprintf(log_file,"\n");
}

void abort_on_error(const char *message) {
	fprintf(log_file,"Error: %s\nAborting.\n",message);
	exit(EXIT_FAILURE);
}

void abort_on_error_f(const char *format, ...) {
	fprintf(log_file,"Error: ");
    va_list argptr;
    va_start(argptr, format);
    vfprintf(log_file, format, argptr);
    va_end(argptr);
    fprintf(log_file,"\nAborting.\n");
    exit(EXIT_FAILURE);
}

void exit_on_error(const char *message, int status) {
	fprintf(log_file,"Error: %s\nExiting with status %d.",message, status);
	exit(status);
}

uint8_t read8(FILE *fp) {
	uint8_t num;
	if (fread(&num,1,1,fp) < 1) abort_on_error("Failed to read file: int 8");
	return num;
}

uint16_t read16(FILE *fp, short encoding) {
	uint16_t num;
	if(fread(&num,2,1,fp) < 1) abort_on_error("Failed to read file: int 16");
	CONVERT_16(encoding, num);
	return num;
}

uint32_t read32(FILE *fp, short encoding) {
	uint32_t num;
	if(fread(&num,4,1,fp) < 1) abort_on_error("Failed to read file: int 32");
	CONVERT_32(encoding, num);
	return num;
}

void push(list_t *list, int32_t value) {
	struct list_node_t *node = malloc(sizeof(struct list_node_t));
	if (!node) abort_on_error("Could not allocate memory for list node");
	node->value = value;
	node->prev = list->tail;
	node->next = NULL;
	if (list->size == 0)
		list->head = list->tail = node;
	else
		list->tail = list->tail->next = node;
	list->size++;
}

int32_t pop(list_t *list) {
	if (list->size < 1) return -1;
	int32_t value = list->tail->value;
	if (list->size == 1) {
		free(list->head);
		list->head = NULL;
		list->tail = NULL;
	}
	else {
		list->tail = list->tail->prev;
		free(list->tail->next);
		list->tail->next = NULL;
	}
	list->size--;
	return value;
}

int32_t unshift(list_t *list) {
	if (list->size < 1) return -1;
	int32_t value = list->head->value;
	if (list->size == 1) {
		free(list->head);
		list->head = NULL;
		list->tail = NULL;
	}
	else {
		list->head = list->head->next;
		free(list->head->prev);
		list->head->prev = NULL;
	}
	list->size--;
	return value;
}
