/*
	D4Player
	* effects.h
*/

#ifndef _EFFECTS_H
#define _EFFECTS_H

#include "effects.h"
#include "allegro.h"

extern COLOR_MAP g_table_blend;
extern COLOR_MAP g_table_ghost;
extern COLOR_MAP g_table_reverse;
extern COLOR_MAP g_table_lightest;
extern COLOR_MAP g_table_darkest;
extern COLOR_MAP g_table_add;
extern COLOR_MAP g_table_add_pin;
extern COLOR_MAP g_table_subtract;
extern COLOR_MAP g_table_sub_pin;
extern COLOR_MAP g_table_tint;

void create_effects();

#endif
