/*
	D4Player
	* text.h
*/

#ifndef _TEXT_H
#define _TEXT_H

#include <stdio.h>

#define NBSP "\u00A0"

// Mac OS Roman table
#define MAC_ROMAN_START 128
#define MAC_ROMAN_TABLE {\
	 "Ä", "Å", "Ç", "É", "Ñ", "Ö", "Ü", "á", "à", "â", "ä", "ã", "å", "ç", "é", "è",\
	 "ê", "ë", "í", "ì", "î", "ï", "ñ", "ó", "ò", "ô", "ö", "õ", "ú", "ù", "û", "ü",\
	 "†", "°", "¢", "£", "§", "•", "¶", "ß", "®", "©", "™", "´", "¨", "≠", "Æ", "Ø",\
	 "∞", "±", "≤", "≥", "¥", "µ", "∂", "∑", "∏", "π", "∫", "ª", "º", "Ω", "æ", "ø",\
	 "¿", "¡", "¬", "√", "ƒ", "≈", "∆", "«", "»", "…",NBSP, "À", "Ã", "Õ", "Œ", "œ",\
	 "–", "—", "“", "”", "‘", "’", "÷", "◊", "ÿ", "Ÿ", "⁄", "¤", "‹", "›", "ﬁ", "ﬂ",\
	 "‡", "·", "‚", "„", "‰", "Â", "Ê", "Á", "Ë", "È", "Í", "Î", "Ï", "Ì", "Ó", "Ô",\
	"", "Ò", "Ú", "Û", "Ù", "ı", "ˆ", "˜", "¯", "˘", "˙", "˚", "¸", "˝", "˛", "ˇ" \
}

extern const char *MAC_ROMAN[];

void print_mac_roman(const char* str);
void fprint_mac_roman(FILE *fp, const char* str);
void fprint_mac_roman_n(FILE *fp, const char* str, int len);

#endif
