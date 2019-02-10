/*
	D4Player
	* player.c
*/

#include "player.h"
#include "../common.h"
#include "../extract.h"
#include "../director/movie.h"
#include "../director/palette.h"
#include "../director/cast.h"
#include "../director/clut.h"
#include "../director/packbits.h"

#include "allegro.h"
#include "effects.h"
#include "patterns.h"
#include "shapes.h"
#include "trans_fx.h"
#include "pal_fx.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
// #include <time.h>

// Loads the image casts from the cast list
cast_t *load_cast(movie_t *movie) {
	cast_t *cast_list = calloc(movie->castlist.size, sizeof(cast_t));
	if (!cast_list) abort_on_error("Could not allocate memory");
	
	int32_t cast_num;
	for (cast_num = 0; cast_num < movie->castlist.size; cast_num++) {
		
		int32_t cast_index = movie->castlist.entries[cast_num];
		
		if (cast_index == 0) continue;
		if(seek_movie(movie, cast_index)) continue;
		
		cast_t cast;
		if(read_cast(&cast, movie->file)) {
			error_f("Failed to read cast: %d", cast_index);
			continue;
		}
		if (
			cast.header.type != CAST_TYPE_IMAGE && 
			cast.header.type != CAST_TYPE_SHAPE
		) continue;
		
		cast_list[cast_num] = cast;
	}
	return cast_list;
}

// Loads the bitmaps from the cast list
BITMAP **load_cast_images(movie_t *movie, cast_t *cast_list) {
	BITMAP **cast_image = calloc(movie->castlist.size, sizeof(BITMAP *));
	if (!cast_image) abort_on_error("Could not allocate memory");
	
	int32_t cast_num;
	for (cast_num = 0; cast_num < movie->castlist.size; cast_num++) {
		
		int32_t cast_index = movie->castlist.entries[cast_num];
		if (cast_index == 0) continue;
		
		cast_t cast = cast_list[cast_num];
		if (cast.header.type != CAST_TYPE_IMAGE) continue;
		
		int32_t bitd_index = keys_get_elem(&movie->keys, cast_index, "BITD");
		
		if (bitd_index < 0) {
			error_f("Cast %d on index %d does not associate to a BITD block", cast_num+1, cast_index);
			continue;
		}
		
		if(seek_movie(movie, bitd_index)) continue;
		
		if (read32(movie->file, movie->encoding) != MKTAG("BITD")) {
			error_f("Wrong header for BITD block on index %d", bitd_index);
			continue;
		}
		uint32_t size = read32(movie->file, movie->encoding);
		
		int32_t width = cast.image.bounds.right - cast.image.bounds.left;
		int32_t width2 = cast.image.unknown01 & 0x7FF;
		int32_t height = cast.image.bounds.bottom - cast.image.bounds.top;
		
		// 1-bit images' width is multiple of 16
		if (cast.image.bitdepth == 1) {
			width = ((width-1)/16+1)*16;
		}
		// 4-bit images' width is multiple of 4
		else if (cast.image.bitdepth == 4) {
			width = ((width-1)/4+1)*4;
		}
		else {
			if (width2 - width == 1) {
				width = width2;
			}
		}
		
		char tmp_filename[255];
		sprintf(tmp_filename, "%s/tmp_XXXXXX", temp_dir);
		int tmp_file_d = mkstemp(tmp_filename);
		if (tmp_file_d < 0) abort_on_error("Could not create temporary file");
		
		FILE *tmp_file = fdopen(tmp_file_d,"wb+");
		if (!tmp_file) abort_on_error("Could not open temporary file for writing.");
		
		// Detects whether the image is PackBits compressed or not
		if (size == height * width * cast.image.bitdepth / 8) {
			unpack_packbits_1(movie->file, tmp_file, size);
		}
		else switch(cast.image.bitdepth) {
			case 1:
			case 4:
			case 8:
				size = unpack_packbits_8(movie->file, tmp_file, size);
				break;
			case 24:
				size = unpack_packbits_24(movie->file, tmp_file, size, width);
				break;
			case 32:
				size = unpack_packbits_32(movie->file, tmp_file, size, width);
				break;
			default:
				error_f("Unsupported bitdepth: %d", cast.image.bitdepth);
				break;
		}
		
		// Rewind output for reading
		fseek(tmp_file, 0, SEEK_SET);
		
		int32_t real_width = cast.image.bounds.right - cast.image.bounds.left;
		int32_t real_height = cast.image.bounds.bottom - cast.image.bounds.top;
		
		BITMAP *bmp = create_bitmap(real_width, real_height);
		if (!bmp) {
			abort_on_error("Could not create bitmap");
		}
		
		clear_to_color(bmp, 0);
		
		if (cast.image.bitdepth == 1) {
			uint8_t p, mask;
			int color;
			int x, y;
			for (y=0; y<height; y++)
			for (x=0; x<width;)
			{
				if (fread(&p,1,1,tmp_file) < 1) abort_on_error("Failed to read temporary file");
				for (mask = 0x80; mask; mask>>=1) {
					if (p&mask)
						color = 255;
					else
						color = 0;
					if (x < real_width && y < real_height)
						putpixel(bmp, x, y, color);
					x++;
				}
			}
		}
		else if (cast.image.bitdepth == 4) {
			warning("4-bit image cast");
			uint8_t pixel;
			int color;
			int x, y;
			for (y=0; y<height; y++)
			for (x=0; x<width; x++)
			{
				if (fread(&pixel, 1, 1, tmp_file) < 1) abort_on_error("Failed to read temporary file");
				if (x < real_width && y < real_height) {
					color = (pixel >> 4);
					putpixel(bmp, x, y, color);
				}
				x++;
				if (x < real_width && y < real_height) {
					color = (pixel & 0xf);
					putpixel(bmp, x, y, color);
				}
			}
		}
		else if (cast.image.bitdepth == 8) {
			pixel8_t pixel;
			int color;
			int x, y;
			for (y=0; y<height; y++)
			for (x=0; x<width; x++)
			{
				if (fread(&pixel, 1, 1, tmp_file) < 1) abort_on_error("Failed to read temporary file");
				if (x < real_width && y < real_height) {
					color = pixel.index;
					putpixel(bmp, x, y, color);
				}
			}
		}
		else if (cast.image.bitdepth == 24) {
			warning("24-bit image cast");
			pixel24_t pixel;
			int color;
			int x, y;
			for (y=0; y<height; y++)
			for (x=0; x<width; x++)
			{
				if (fread(&pixel, 3, 1, tmp_file) < 1) abort_on_error("Failed to read temporary file");
				if (x < real_width && y < real_height) {
					color = makecol(pixel.r, pixel.g, pixel.b);
					putpixel(bmp, x, y, color);
				}
			}
		}
		else if (cast.image.bitdepth == 32) {
			warning("32-bit image cast");
			pixel32_t pixel;
			int color;
			int x, y;
			for (y=0; y<height; y++)
			for (x=0; x<width; x++)
			{
				if (fread(&pixel, 4, 1, tmp_file) < 1) abort_on_error("Failed to read temporary file");
				if (x < real_width && y < real_height) {
					color = makecol(pixel.r, pixel.g, pixel.b);
					putpixel(bmp, x, y, color);
				}
			}
		}
		cast_image[cast_num] = bmp;
		fclose(tmp_file);
		remove(tmp_filename);
	}
	
	return cast_image;
}

// Deallocate bitmaps
void free_cast_images(BITMAP **cast_image, movie_t *movie) {
	int32_t i;
	for (i = 0; i < movie->castlist.size; i++) {
		if (!cast_image[i]) continue;
		destroy_bitmap(cast_image[i]);
		cast_image[i] = NULL;
	}
	free(cast_image);
}

int player(movie_t *movie) {
	
	if (start_allegro(movie->info.width, movie->info.height) != 0) {
		error("Failed to start graphics.");
		return 1;
	}
	
	clear_to_color(buffer, 255);
	textout_centre_ex(buffer, font, "Loading...", SCREEN_W/2, SCREEN_H/2-4, 0, 255);
	flush_buffer();
	
	palette_t default_palette;
	load_palette_file(&default_palette, "palettes/01.pal");
	set_current_palette(&default_palette);
	create_effects();
	load_patterns();
	
	cast_t *cast_list = load_cast(movie);
	BITMAP **cast_image = load_cast_images(movie, cast_list);
	extract_sound(movie);
	
	SAMPLE *samples[2] = {NULL,NULL};
	int last_snd[2] = {0,0};
	int voice[2];
	
	// clock_t t1 = clock(), t2;
	int frame, fps = movie->info.framerate, delay = 0, stagebg = movie->info.bgcolor;
	int paused = 0;
	for (frame = 0; frame < movie->score.frame_count; frame++) {
		
		//frame = 65;
		
		if (key[KEY_ESC]) break;
		if (key[KEY_SPACE]) {
			paused = !paused;
			while(key[KEY_SPACE]) rest(100);
		}
		if (paused) {
			rest(100);
			continue;
		}
		
		clear_to_color(buffer, stagebg);
		
		mainchannel_frame_t main_channel = movie->score.mainchannel[frame];
		palette_frame_t palette_channel = movie->score.palette[frame];
		
		if (palette_channel.palette_cast) {
			int32_t clut_index = palette_channel.palette_cast-movie->info.castlist_start;
			if (clut_index < 0 || clut_index >= movie->castlist.size) {
				warning("Palette cast is out of bounds");
				set_current_palette(&default_palette);
			}
			else {
				clut_t clut;
				clut_index = movie->castlist.entries[clut_index];
				clut_index = keys_get_elem(&movie->keys, clut_index, "CLUT");
				if (!seek_movie(movie, clut_index)
				&&  !read_clut(&clut, movie->file, movie->encoding))
				{
					set_current_palette(&clut.palette);
				}
				else {
					warning("Failed to load palette");
					set_current_palette(&default_palette);
				}
			}
		}
		
		// Plays the sound channels
		int i;
		for (i=0; i<2; i++) {
			int32_t snd_cast;
			if (i==0) snd_cast = main_channel.sound1_cast;
			if (i==1) snd_cast = main_channel.sound2_cast;
			if (snd_cast != last_snd[i]) {
				if (samples[i]) {
					stop_sample(samples[i]);
					destroy_sample(samples[i]);
					samples[i] = NULL;
				}
				if (snd_cast) {
					if (snd_cast > movie->castlist.size) {
						error_f("Sound channel references cast out of bounds: %d", snd_cast);
						continue;
					}
					int32_t cast_index = movie->castlist.entries[snd_cast-1];
					if (cast_index == 0) {
						error_f("Sound channel references an empty cast: %d", snd_cast);
						continue;
					}
					int32_t elem_index = keys_get_elem(&movie->keys, cast_index, "snd ");
					if (elem_index < 0) {
						error_f("Sound cast does not reference a sound block: %d", snd_cast);
						continue;
					}
					char filename[255];
					sprintf(filename,"%s/%04d.wav", output_dir, elem_index);
					samples[i] = load_sample(filename);
					if (!samples[i]) {
						error_f("Could not load wave sample: %d", elem_index);
						continue;
					}
					voice[i] = play_sample(samples[i], 255, 127, 1000, 0);
					if (voice[i] < 0) {
						error_f("Could not allocate voice for cast %d", snd_cast);
					}
					debug_f("Playing sample %d on voice %d\n", elem_index, voice[i]);
				}
			}
			last_snd[i] = snd_cast;
		}
		
		delay = 0;
		if ((int8_t) main_channel.timing > 0) {
			fps = main_channel.timing;
		}
		else if ((uint8_t)main_channel.timing >= 196 && (uint8_t)main_channel.timing <= 255) {
			delay = 0 - ((int8_t) main_channel.timing);
		}
		
		int channel;
		for (channel = 0; channel < movie->score.channel_count; channel++) {
			sprite_frame_t f = movie->score.frames[frame * (movie->score.channel_count) + channel];
			
			if (f.type == 4096) {
				sprite_frame_t f2;
				f2.type = 1;
				f2.color = f.color;
				f2.ink_type = f.top;
				f2.cast = f.ink_type;
				f2.top = f.height;
				f2.left = f.width;
				f2.height = f.flag1;
				f2.width = f.flag2;
				f2.flag1 = f.cast;
				f2.flag2 = f.left;
				f = f2;
			}
			
			int width = f.width,
				height = f.height,
				top = f.top,
				left = f.left,
				ink_type = FRAME_INK_TYPE(f),
				bgcolor  = FRAME_BG_COLOR(f),
				fgcolor  = FRAME_FG_COLOR(f);
			
			BITMAP *bmp = NULL;
			short destroy_bmp = 1;
			
			switch (f.type) {
				case FRAME_TYPE_RECTANGLE:
					if (is_opaque_shape(ink_type))
						draw_shape(buffer, left, top, SHAPE_RECTANGLE, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, 1);
					else
						bmp = make_shape(SHAPE_RECTANGLE, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, 1);
					break;
				case FRAME_TYPE_ROUND_RECTANGLE:
					if (is_opaque_shape(ink_type))
						draw_shape(buffer, left, top, SHAPE_ROUND_RECTANGLE, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, 1);
					else
						bmp = make_shape(SHAPE_ROUND_RECTANGLE, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, 1);
					break;
				case FRAME_TYPE_ELLIPSE:
					if (is_opaque_shape(ink_type))
						draw_shape(buffer, left, top, SHAPE_ELLIPSE, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, 1);
					else
						bmp = make_shape(SHAPE_ELLIPSE, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, 1);
					break;
				case FRAME_TYPE_LINE_TL_BR:
					if (is_opaque_shape(ink_type))
						draw_shape(buffer, left, top, SHAPE_LINE_TL_BR, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, 1);
					else
						bmp = make_shape(SHAPE_LINE_TL_BR, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, 1);
					break;
				case FRAME_TYPE_LINE_BL_TR:
					if (is_opaque_shape(ink_type))
						draw_shape(buffer, left, top, SHAPE_LINE_BL_TR, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, 1);
					else
						bmp = make_shape(SHAPE_LINE_BL_TR, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, 1);
					break;
				case FRAME_TYPE_SHAPE_CAST:
				{
					int32_t cast = f.cast-movie->info.castlist_start;
					if (cast < 0 || cast >= movie->castlist.size) {
						warning_f("Score is referencing a cast out of range: %d", f.cast);
						continue;
					}
					cast_shape_t shape = cast_list[cast].shape;
					
					switch(shape.type) {
						case CAST_SHAPE_RECTANGLE:
							if (is_opaque_shape(ink_type))
								draw_shape(buffer, left, top, SHAPE_RECTANGLE, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, shape.filled);
							else
								bmp = make_shape(SHAPE_RECTANGLE, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, shape.filled);
							break;
						case CAST_SHAPE_ELLIPSE:
							if (is_opaque_shape(ink_type))
								draw_shape(buffer, left, top, SHAPE_ELLIPSE, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, shape.filled);
							else
								bmp = make_shape(SHAPE_ELLIPSE, f.width, f.height, bgcolor, fgcolor, FRAME_BORDER_WIDTH(f)-1, f.cast, shape.filled);
							break;
					}
				}
					break;
				case FRAME_TYPE_IMAGE_CAST:				
				{
					int32_t cast = f.cast-movie->info.castlist_start;
					if (cast < 0 || cast >= movie->castlist.size) {
						warning_f("Score is referencing a cast out of range: %d", f.cast);
						continue;
					}
					bmp = cast_image[cast];
					destroy_bmp = 0;
					
					if (!bmp) {
						warning_f("Cast %d image was not loaded", f.cast);
						continue;
					}
					cast_image_t img = cast_list[cast].image;
					
					// Disabling upscaling.
					// For some reason, images are not scaled correctly on the score.
					if (width > bmp->w) width = bmp->w;
					if (height > bmp->h) height = bmp->h;
					
					top = f.top - img.center_y + img.bounds.top;
					left = f.left - img.center_x + img.bounds.left;
					
					// Repositions x and y coords, as the images are scaled around their centers
					if (height != img.bounds.bottom - img.bounds.top) {
						top  += (img.bounds.bottom - img.bounds.top - height) * (img.center_y - img.bounds.top) / (img.bounds.bottom - img.bounds.top);
					}
					if (width != img.bounds.right - img.bounds.left) {
						left += (img.bounds.right - img.bounds.left - width) * (img.center_x - img.bounds.left) / (img.bounds.right - img.bounds.left);
					}
				}
					break;
				case FRAME_TYPE_EMPTY:
					break;
				case FRAME_TYPE_TEXT:
				case FRAME_TYPE_BUTTON:
				case FRAME_TYPE_CHECKBOX:
				case FRAME_TYPE_RADIO_BUTTON:
					warning_f("Frame type not implemented: %d", f.type);
					break;
				default:
					warning_f("Unknown frame type: %d", f.type);
					break;
			}
			
			if (bmp == NULL) continue;
			
			// Creates a reverse copy of bitmap
			if (
				ink_type == INK_TYPE_NOT_COPY ||
				ink_type == INK_TYPE_NOT_TRANSPARENT ||
				ink_type == INK_TYPE_NOT_REVERSE ||
				ink_type == INK_TYPE_NOT_GHOST
			) {
				BITMAP *copy = create_bitmap(bmp->w, bmp->h);
				blit(bmp, copy, 0, 0 , 0, 0, bmp->w, bmp->w);
				
				if (destroy_bmp)
					destroy_bitmap(bmp);
				
				bmp = copy;
				destroy_bmp = 1;
			}
			
			// COPY / NOT COPY
			if (
				ink_type == INK_TYPE_COPY ||
				ink_type == INK_TYPE_NOT_COPY
			) {
				if (bgcolor == 0 && fgcolor == 255)
					stretch_blit(bmp, buffer, 0, 0, bmp->w, bmp->h, left, top, width, height);
				else {
					BITMAP *copy = create_bitmap(bmp->w, bmp->h);
					clear_to_color(copy, bgcolor);
					color_map = &g_table_tint;
					draw_lit_sprite(copy, bmp, 0, 0, fgcolor);
					stretch_blit(copy, buffer, 0, 0, bmp->w, bmp->h, left, top, width, height);
					destroy_bitmap(copy);
				}
			}
			// MATTE
			else if (ink_type == INK_TYPE_MATTE) {
				BITMAP *mask = create_bitmap(bmp->w+2, bmp->h+2);
				BITMAP *copy = create_bitmap(width, height);
				// Creates a mask of the bitmap
				clear_to_color(mask, 1);
				draw_character_ex(mask, bmp, 1, 1, 2, 1);
				floodfill(mask, 0, 0, 0);
				// Copy the mask to the final size and draws to buffer
				stretch_blit(mask, copy, 1, 1, bmp->w, bmp->h, 0, 0, width, height);
				draw_character_ex(buffer, copy, left, top, bgcolor, -1);
				if (fgcolor == 255) {
					masked_stretch_blit(bmp, buffer, 0, 0, bmp->w, bmp->h, left, top, width, height);
				}
				else {
					// Draws outline on fgcolor
					stretch_blit(bmp, copy, 0, 0, bmp->w, bmp->h, 0, 0, width, height);
					draw_character_ex(buffer, copy, left, top, fgcolor, -1);
				}
				// Clear temp bitmaps
				destroy_bitmap(copy);
				destroy_bitmap(mask);
			}
			// BACKGROUND TRANSPARENT
			// TRANSPARENT/ NOT TRANSPARENT
			else if (
				ink_type == INK_TYPE_BKGND_TRANSPARENT ||
				ink_type == INK_TYPE_TRANSPARENT ||
				ink_type == INK_TYPE_NOT_TRANSPARENT
			) {
				if (bgcolor == 0 && fgcolor == 255)
					masked_stretch_blit(bmp, buffer, 0, 0, bmp->w, bmp->h, left, top, width, height);
				else {
					BITMAP *copy = create_bitmap(bmp->w, bmp->h);
					clear_to_color(copy, 0);
					color_map = &g_table_tint;
					draw_lit_sprite(copy, bmp, 0, 0, fgcolor);
					masked_stretch_blit(copy, buffer, 0, 0, bmp->w, bmp->h, left, top, width, height);
					destroy_bitmap(copy);
				}
			}
			else {
				color_map = NULL;
				switch (ink_type) {
					case INK_TYPE_BLEND   : color_map = &g_table_blend   ; break;
					case INK_TYPE_NOT_GHOST:
					case INK_TYPE_GHOST   : color_map = &g_table_ghost   ; break;
					case INK_TYPE_NOT_REVERSE:
					case INK_TYPE_REVERSE : color_map = &g_table_reverse ; break;
					case INK_TYPE_LIGHTEST: color_map = &g_table_lightest; break;
					case INK_TYPE_DARKEST : color_map = &g_table_darkest ; break;
					case INK_TYPE_ADD     : color_map = &g_table_add     ; break;
					case INK_TYPE_ADD_PIN : color_map = &g_table_add_pin ; break;
					case INK_TYPE_SUBTRACT: color_map = &g_table_subtract; break;
					case INK_TYPE_SUB_PIN : color_map = &g_table_sub_pin ; break;
					default:
						error_f("Inktype not defined: %d", ink_type);
						break;
				}
				if (color_map) {
					BITMAP *tmp = create_bitmap(width, height);
					stretch_blit(bmp, tmp, 0, 0, bmp->w, bmp->h, 0, 0, width, height);
					draw_trans_sprite(buffer, tmp, left, top);
					destroy_bitmap(tmp);
				}
			}
			
			if (destroy_bmp) {
				destroy_bitmap(bmp);
			}
		}
		#ifdef DEBUG
		if (delay)
			textprintf_ex(buffer, font, 10, 10, 255, 0, "Frame:%4d/%4d | Fps: %2d - DELAY: %d s", frame+1, movie->score.frame_count, fps, delay);
		else
			textprintf_ex(buffer, font, 10, 10, 255, 0, "Frame:%4d/%4d | Fps: %2d", frame+1, movie->score.frame_count, fps);
		#endif
		
		// t2 = clock();
		int diff = 0; // (t2-t1) * 1000 / CLOCKS_PER_SEC;
		// t1 = t2;
		
		flush_buffer();
		if (key[KEY_F]) continue;
		
		if (delay) delay = 1000*delay - diff;
		else delay = 1000/fps - diff;
		
		if (delay > 0) rest(delay);
		else rest(1);
	}
	
	if (frame < 30)
		rest(500*(30-frame)/30);
	else
		rest(200);
	
	free_cast_images(cast_image, movie);
	cast_image = NULL;
	
	free(cast_list);
	free_patterns();
	
	stop_allegro();
	return 0;
}
