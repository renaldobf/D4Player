/*
	D4Player
	* text.c
*/

#include "text.h"
#include <stdio.h>

const char *MAC_ROMAN[] = MAC_ROMAN_TABLE;

void print_mac_roman(const char* str) {
	fprint_mac_roman(stdout, str);
}

void fprint_mac_roman(FILE *fp, const char* str) {
	for (; *str != '\0'; str++)
	{
		if ((unsigned char) *str < MAC_ROMAN_START)
			fprintf(fp, "%c", *str);
		else
			fprintf(fp, "%s", MAC_ROMAN[(unsigned char) *str - MAC_ROMAN_START]);
	}
}

void fprint_mac_roman_n(FILE *fp, const char* str, int len) {
	for (; len > 0 && *str != '\0'; str++, len--)
	{
		if ((unsigned char) *str < MAC_ROMAN_START)
			fprintf(fp, "%c", *str);
		else
			fprintf(fp, "%s", MAC_ROMAN[(unsigned char) *str - MAC_ROMAN_START]);
	}
}
