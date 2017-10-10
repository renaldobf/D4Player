/*
	D4Player
	* imgwrap.c
*/

#include "imgwrap.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(ENABLE_PNG) && ENABLE_PNG != 0
#include <png.h>
#endif

#if defined(ENABLE_PNG) && ENABLE_PNG != 0
	int (*write_img)(FILE*, unsigned char*, int, int, int, unsigned char *, int) = write_png;
#else
	int (*write_img)(FILE*, unsigned char*, int, int, int, unsigned char *, int) = write_ppm;
#endif

int write_png(
	FILE *fp, unsigned char *buffer, int width, int height, int color_depth,
	unsigned char *palette, int palette_size
) {
#if !defined(ENABLE_PNG) || ENABLE_PNG == 0
	warning("PNG output was not enabled on compilation");
	return -1;
#else
	// Safity check on the inputs
	if (fp == NULL) {
		error("File pointer is null");
		return -1;
	}
	if (buffer == NULL) {
		error("Image buffer pointer is null");
		return -1;
	}
	if (width < 1) {
		error("Width is smaller than 1");
		return -1;
	}
	if (height < 1) {
		error("Height is smaller than 1");
		return -1;
	}
	if (color_depth == 8) {
		if (palette == NULL) {
			error("Palete pointer is null");
			return -1;
		}
		if (palette_size < 1 || palette_size > 256 || palette_size > PNG_MAX_PALETTE_LENGTH) {
			error_f("Invalid palette size: %d", palette_size);
			return -1;
		}
	}
	
	int status = 0;
	// PNG structures
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	png_color *png_palette = NULL;
	// PNG constants
	int color_type;
	int bit_depth = 8; // 8 bits per channel
	
	switch (color_depth) {
		case  8: color_type = PNG_COLOR_TYPE_PALETTE;   break;
		case 24: color_type = PNG_COLOR_TYPE_RGB;       break;
		case 32: color_type = PNG_COLOR_TYPE_RGB_ALPHA; break;
		default:
			error_f("Unsuported color depth: %d", color_depth);
			return -1;
			break;
	}
	
	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		error("Could not allocate PNG write struct");
		status = -1;
		goto clean_up;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		error("Could not allocate PNG info struct");
		status = -1;
		goto clean_up;
	}
	
	// Setup Exception handling: libpng will jump here if an error occur during writing :)
	if (setjmp(png_jmpbuf(png_ptr))) {
		error("Error during PNG creation");
		status = -1;
		goto clean_up;
	}
	
	// Sets the header
	png_set_IHDR(
		png_ptr, info_ptr, width, height, bit_depth, color_type,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE,
		PNG_FILTER_TYPE_BASE
	);
	// Sets the palette
	if (color_depth == 8) {
		png_palette = (png_color *) png_malloc(png_ptr, palette_size * sizeof(png_color));
		
		if (png_palette == NULL) {
			error("Could not allocate PNG palette");
			status = -1;
			goto clean_up;
		}
		int i;
		for (i=0; i < palette_size; i++) {
			png_color *color = &png_palette[i];
			color->red   = palette[i*3+0];
			color->green = palette[i*3+1];
			color->blue  = palette[i*3+2];
		}
		
		png_set_PLTE(png_ptr, info_ptr, png_palette, palette_size);
	}
	
	// Inits writing
	png_init_io(png_ptr, fp);
	// Writes header
	png_write_info(png_ptr, info_ptr);
	// Allocate memory for one row
	int bytes_per_pixel = (color_depth/bit_depth);
	int row_size = width*bytes_per_pixel;
	row = (png_bytep) malloc(row_size * sizeof(png_byte));
	// Write image data
	int y;
	for (y=0 ; y<height ; y++) {
		memcpy(row, buffer+(y*row_size), row_size);
		png_write_row(png_ptr, row);
	}
	// End write
	png_write_end(png_ptr, NULL);

	clean_up:
	if (png_palette != NULL) png_free(png_ptr, png_palette);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
	if (row != NULL) free(row);

	return status;
#endif
}

int write_ppm(
	FILE *fp, unsigned char *buffer, int width, int height, int color_depth,
	unsigned char *palette, int palette_size
) {
	// Safity check on the inputs
	if (fp == NULL) {
		error("File pointer is null");
		return -1;
	}
	if (buffer == NULL) {
		error("Image buffer pointer is null");
		return -1;
	}
	if (width < 1) {
		error("Width is smaller than 1");
		return -1;
	}
	if (height < 1) {
		error("Height is smaller than 1");
		return -1;
	}
	switch (color_depth) {
		case 8:
		case 24:
		case 32:
			break;
		default:
			error("Unsuported color depth");
			return -1;
			break;
	}
	if (color_depth == 8) {
		if (palette == NULL) {
			error("Palete pointer is null");
			return -1;
		}
		if (palette_size < 1 || palette_size > 256) {
			error("Invalid palette size");
			return -1;
		}
	}
	
	fprintf(fp,"P6\n%d %d %d\n", width, height, 255);
	
	switch (color_depth) {
		case 8:
			{
				unsigned char *pixel = buffer;
				int length;
				for (length = width*height; length>0; length--, pixel++) {
					fwrite(palette+3*(*pixel),3,1,fp);
				}
			}
			break;
		case 24:
			{
				unsigned char *pixel = buffer;
				int length;
				for (length = width*height; length>0; length--, pixel+=3) {
					fwrite(pixel,3,1,fp);
				}	
			}
			break;
		case 32:
			{ // Ignoring alpha channel
				unsigned char *pixel = buffer;
				int length;
				for (length = width*height; length>0; length--, pixel+=4) {
					fwrite(pixel,3,1,fp);
				}	
			}
			break;
	}
	
	return 0;
}
