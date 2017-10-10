/*
	D4Player
	* allegro.c
*/

#include "allegro.h"
#include "../common.h"

// Global vars
BITMAP *buffer;
void (*flush_buffer)();
void (*clone_buffer)();

// Local vars
BITMAP *video_page[2];
int is_flip;
int offscreen_page;

void flip_page() {
	show_video_bitmap(buffer);
	buffer = video_page[offscreen_page = 1 - offscreen_page];
}

void blit_buffer() {
	vsync();
	blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
}

void stretch_buffer() {
	vsync();
	stretch_blit(buffer, screen, 0, 0, buffer->w, buffer->h, 0, 0, SCREEN_W, SCREEN_H);
}

// Starts up graphical envoirment
// Return 0 on success, 1 otherwise
int start_allegro(int width, int height) {
    
    set_uformat(U_UTF8);
    
    if (allegro_init()) {
		error("Could not start Allegro engine");
		return 1;
	}
	if (install_timer()) {
		error("Could not install Allegro timer");
		return 1;
	}
	if(install_keyboard()) {
		error("Failed to install keyboard");
		return 1;
	}
	/*
	if (install_mouse() < 0) {
		error("Failed to install mouse");
		return 1;
	}
	*/
	reserve_voices(2,0);
	if(install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL)) {
		error("Failed to install sound");
		error(allegro_error);
	}
    
    set_color_depth(COLOR_DEPTH);
	
	is_flip = 1;
	int fallback = 0;
	
    if (is_flip) {
		// Tries to set up graphisc with given resolution, using page flip
		if (set_gfx_mode(GFX_AUTODETECT_WINDOWED  , width, height, 0, height*2) < 0)
		if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, width, height, 0, height*2) < 0)
		if (set_gfx_mode(GFX_AUTODETECT           , width, height, 0, height*2) < 0) {
			debug("Failed to set page flip mode");
			debug(allegro_error);
			debug("Turning off page flip");
			is_flip = 0;
		}
		if (is_flip) {
			video_page[0] = create_video_bitmap(SCREEN_W, SCREEN_H);
			video_page[1] = create_video_bitmap(SCREEN_W, SCREEN_H);
			if (video_page[0] && video_page[1]) {
				buffer = video_page[offscreen_page = 0];
				flush_buffer = flip_page;
				// clone_buffer = clone_page;
			}
			else {
				debug("Could not create page bitmaps");
				debug("Turning off page flip");
				destroy_bitmap(video_page[0]);
				destroy_bitmap(video_page[1]);
				is_flip = 0;
			}
		}
	}
	if (!is_flip) {
		// Tries to set up graphics with given resolution, using RAM double buffer
		debug("Page flip is off");
		if (set_gfx_mode(GFX_AUTODETECT_WINDOWED  , width, height, 0, 0) < 0)
		if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, width, height, 0, 0) < 0)
		if (set_gfx_mode(GFX_AUTODETECT           , width, height, 0, 0) < 0) {
			error("Could not set the graphics mode");
			error(allegro_error);
			// return 1;
			fallback = 1;
		}
		if (!fallback) {
			buffer = create_bitmap(SCREEN_W, SCREEN_H);
			if (buffer) {
				flush_buffer = blit_buffer;
				// clone_buffer = clone_blit;
			}
			else {
				error("Could not create buffer bitmap");
				// return 1;
				fallback = 1;
			}
		}
	}
	if (fallback) {
		debug("Entering fallback mode");
		int desktop_w, desktop_h;
		get_desktop_resolution(&desktop_w, &desktop_h);
		if (desktop_w < width && desktop_h < height) {
			warning("Desktop resolution is smaller than movie stage size");
		}
		// Tries to set up graphics to desktop resolution, using RAM double buffer
		if (set_gfx_mode(GFX_AUTODETECT_WINDOWED  , desktop_w, desktop_h, 0, 0) < 0)
		if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, desktop_w, desktop_h, 0, 0) < 0)
		if (set_gfx_mode(GFX_AUTODETECT           , desktop_w, desktop_h, 0, 0) < 0) {
			error("Could not set the graphics mode");
			error(allegro_error);
			return 1;
		}
		buffer = create_bitmap(width, height);
		if (buffer) {
			flush_buffer = stretch_buffer;
			// clone_buffer = clone_blit;
		}
		else {
			error("Could not create buffer bitmap");
			return 1;
		}
	}
		
	set_window_title(WINDOW_TITLE);
	
	return 0;
}

// Finishes graphical envoirment
void stop_allegro() {
	if (is_flip) {
		destroy_bitmap(video_page[0]);
		destroy_bitmap(video_page[1]);
	}
	else {
		destroy_bitmap(buffer);
	}
    allegro_exit();
}

// Draws rounded rectangle
void rrect(BITMAP *bmp, int x1, int y1, int x2, int y2, int r, int color) {
	if (r > (abs(x2-x1)+1)/2) r = (abs(x2-x1)+1)/2;
	if (r > (abs(y2-y1)+1)/2) r = (abs(y2-y1)+1)/2;
	
	hline(bmp, x1+r, y1, x2-r, color); 
	hline(bmp, x1+r, y2, x2-r, color); 
	vline(bmp, x1, y1+r, y2-r, color);
	vline(bmp, x2, y1+r, y2-r, color);
	
    arc(bmp, x1+r, y1+r,  itofix(64), itofix(128), r, color);
    arc(bmp, x2-r, y1+r,   itofix(0),  itofix(64), r, color);
    arc(bmp, x1+r, y2-r, itofix(128), itofix(192), r, color);
    arc(bmp, x2-r, y2-r, itofix(192), itofix(256), r, color);
}

// Draws filled rounded rectangle
void rrectfill(BITMAP *bmp, int x1, int y1, int x2, int y2, int r, int color) {
	rectfill(bmp, x1, y1, x2, y2, color);
}
