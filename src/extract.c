/*
	D4Player
	* extract.c
*/

#include "extract.h"
#include "common.h"
#include "director/movie.h"
#include "director/sound.h"
#include "director/cast.h"
#include "director/palette.h"
#include "director/clut.h"
#include "director/packbits.h"
#include "director/text.h"
#include "imgwrap.h"
#include "wave.h"

#include <stdio.h>
#include <stdlib.h>

void extract_image(movie_t *movie, int32_t clut_index) {
	FILE *fp = movie->file;
	int32_t i = -1;
	while ((i = mmap_search(&movie->mmap, "BITD", ++i)) >= 0) {
		int32_t cast_index;
		cast_t cast;
		
		cast_index = keys_get_cast(&movie->keys, i);
		if (seek_movie(movie, cast_index))
			continue;
		if (read_cast(&cast, fp))
			continue;
		if (cast.header.type != CAST_TYPE_IMAGE)
			continue;
			
		if (cast.image.bitdepth == 4) {
			warning("Skipping 4-bit image cast");
			continue;
		}
			
		if (seek_movie(movie, i))
			continue;
		
		if (read32(movie->file, movie->encoding) != MKTAG("BITD")) {
			error_f("Wrong header for BITD block on index %d", i);
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
		if (tmp_file_d < 0)
			abort_on_error_f("Could not create temporary file: %s", tmp_filename);
		
		FILE *tmp_file = fdopen(tmp_file_d,"wb+");
		if (!tmp_file)
			abort_on_error_f("Could not open temporary file for writing: %s", tmp_filename);
		
		// Detects whether the image is PackBits compressed or not
		if (size == height * width * cast.image.bitdepth / 8) {
			switch(cast.image.bitdepth) {
				case 1:
				case 4:
				case 8:
				case 24:
					unpack_packbits_1(movie->file, tmp_file, size);
					break;
				case 32:
					unpack_packbits_2(movie->file, tmp_file, size);
					break;
				default:
					abort_on_error_f("Unsupported bitdepth: %d", cast.image.bitdepth);
					break;
				}
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
				abort_on_error_f("Unsupported bitdepth: %d", cast.image.bitdepth);
				break;
		}
		
		// Rewind output for reading
		fseek(tmp_file, 0, SEEK_SET);
		
		int32_t real_width = cast.image.bounds.right - cast.image.bounds.left;
		int32_t real_height = cast.image.bounds.bottom - cast.image.bounds.top;
		
		uint8_t *buffer = NULL;
		palette_t palette;
		
		if (cast.image.bitdepth == 1) {
			debug("1-bit image cast");
			buffer = malloc(real_width*real_height);
			palette.size = 2;
			palette.palette[0] = (pixel24_t){255,255,255};
			palette.palette[1] = (pixel24_t){0,0,0};
			int buffer_index = 0;
			uint8_t p, mask, color;
			int x, y;
			for (y=0; y<height; y++)
			for (x=0; x<width;)
			{
				if (fread(&p,1,1,tmp_file) < 1) abort_on_error("Failed to read temporary file");
				for (mask = 0x80; mask; mask>>=1) {
					if (p&mask)
						color = 1;
					else
						color = 0;
					if (x < real_width && y < real_height)
						buffer[buffer_index++] = color;
					x++;
				}
			}
		}
		/*
		else if (cast.image.bitdepth == 4) {
			// NOT SUPPORTED YET
		}
		*/
		else if (cast.image.bitdepth == 8) {
			debug("8-bit image cast");
			buffer = malloc(real_width*real_height);
			{
				if (clut_index && seek_movie_search(movie, "CLUT", clut_index) == 0) {
					clut_t clut;
					read_clut(&clut, movie->file, movie->encoding);
					palette = clut.palette;
				}
				else
					load_palette_file(&palette, "palettes/01.pal");
			
			}
			int buffer_index = 0;
			pixel8_t pixel;
			int x, y;
			for (y=0; y<height; y++)
			for (x=0; x<width; x++)
			{
				if (fread(&pixel, 1, 1, tmp_file) < 1) abort_on_error("Failed to read temporary file");
				if (x < real_width && y < real_height) {
					buffer[buffer_index++] = pixel.index;
				}
			}
		}
		else if (cast.image.bitdepth == 24) {
			debug("24-bit image cast");
			buffer = malloc(real_width*real_height*3);
			pixel24_t pixel;
			int x, y;
			int buffer_index = 0;
			for (y=0; y<height; y++)
			for (x=0; x<width; x++)
			{
				if (fread(&pixel, 3, 1, tmp_file) < 1) abort_on_error("Failed to read temporary file");
				if (x < real_width && y < real_height) {
					buffer[buffer_index++] = pixel.r;
					buffer[buffer_index++] = pixel.g;
					buffer[buffer_index++] = pixel.b;
				}
			}
		}
		else if (cast.image.bitdepth == 32) {
			debug("32-bit image cast");
			buffer = malloc(real_width*real_height*4);
			pixel32_t pixel;
			int x, y;
			int buffer_index = 0;
			for (y=0; y<height; y++)
			for (x=0; x<width; x++)
			{
				if (fread(&pixel, 4, 1, tmp_file) < 1) abort_on_error("Failed to read temporary file");
				if (x < real_width && y < real_height) {
					buffer[buffer_index++] = pixel.r;
					buffer[buffer_index++] = pixel.g;
					buffer[buffer_index++] = pixel.b;
					buffer[buffer_index++] = pixel.a;
				}
			}
		}
		
		fclose(tmp_file);
		remove(tmp_filename);
		
		char filename[256];
		
		if (write_img == write_png)
			sprintf(filename, "%s/%04d.png", output_dir, i);
		if (write_img == write_ppm)
			sprintf(filename, "%s/%04d.ppm", output_dir, i);
			
		FILE *output = fopen(filename, "wb");
		
		if (!output) {
			error_f("Failed to open file for writing: %s", filename);
			goto clean_up;
		}
		
		int status = write_img(
			output, buffer, real_width, real_height,
			(cast.image.bitdepth == 1 ? 8 : cast.image.bitdepth),
			(unsigned char *) palette.palette, palette.size);
			
		if (status) {
			error_f("Failed to extract image on index %d", i);
			fclose(output);
			output = NULL;
			remove(filename);
		}
		
		clean_up:
		
		free(buffer);
		if (output) fclose(output);
	}
}

void extract_sound(movie_t *movie) {
	FILE *fp = movie->file;
	int32_t i = -1;
	while ((i = mmap_search(&movie->mmap, "snd ",++i)) >= 0) {
		
		if (seek_movie(movie, i)) {
			continue;
		}
		
		sount_t snd;
		if (read_sound(&snd, fp)) {
			error("Error while reading sound block.");
			continue;
		}
		#ifdef DEBUG
		print_sound(&snd);
		#endif
		
		char output_filename[255];
		sprintf(output_filename,"%s/%04d.wav", output_dir, i);
		
		FILE *output = fopen(output_filename, "wb+");
		if (!output)
			abort_on_error_f("Could not open file for writing: %s", output_filename);
		
		write_wave(output, snd.channels, snd.sample_rate, snd.samples, snd.bps, fp);
		
		fclose(output);
	}	
}

void extract_score(movie_t *movie) {
	char filename[255];
	sprintf(filename, "%s/score.csv", output_dir);
	
	FILE *fp = fopen(filename, "w");
	if (!fp) {
		error_f("Could not open file for writing: %s", filename);
		return;
	}
	
	uint16_t *frame;
	int size;
	
	int i, j;
	for (i=0; i<movie->score.frame_count; i++) {
		fprintf(fp,"%d",i+1);
		
		frame = (uint16_t *) &movie->score.mainchannel[i];
		size = 10;
		for (j=0; j<size; j++)
			fprintf(fp,",%d",frame[j]);
			
		frame = (uint16_t *) &movie->score.palette[i];
		size = 10;
		for (j=0; j<size; j++)
			fprintf(fp,",%d",frame[j]);
			
		frame = (uint16_t *) &movie->score.frames[i*movie->score.channel_count];
		size = 10*movie->score.channel_count;
		for (j=0; j<size; j++)
			fprintf(fp,",%d",frame[j]);
		
		fprintf(fp,"\n");
	}
	
	fclose(fp);
}

void extract_script(movie_t *movie) {
	FILE *fp = movie->file;
	int32_t i = -1;
	while ((i = mmap_search(&movie->mmap, "CASt",++i)) >= 0) {
		
		if (seek_movie(movie, i)) {
			continue;
		}
		
		cast_t cast;
		if (read_cast(&cast, fp)) {
			error("Error while reading CASt block.");
			continue;
		}
		if (cast.header.type != CAST_TYPE_SCRIPT) continue;
		
		char output_filename[255];
		sprintf(output_filename,"%s/%04d.lingo", output_dir, i);
		
		FILE *output = fopen(output_filename, "wb");
		if (!output)
			abort_on_error_f("Could not open file for writing: %s", output_filename);
		
		fseek(fp, cast.script.offset, SEEK_SET);
		
		rip_mac(fp, output, cast.script.size);
		
		fclose(output);
	}	
}

void rip(FILE *fp, FILE *output, long length) {
	unsigned char buffer[COPY_BUFFER_SIZE];
	long bytes_read;
	for(;;) {
		if (length < COPY_BUFFER_SIZE)
			bytes_read = fread(buffer,1,length,fp);
		else
			bytes_read = fread(buffer,1,COPY_BUFFER_SIZE,fp);
		if (bytes_read <= 0) break;
		fwrite(buffer,1,bytes_read,output);
		length -= bytes_read;
	}
}

void rip_mac(FILE *fp, FILE *output, long length) {
	unsigned char buffer[COPY_BUFFER_SIZE];
	long bytes_read;
	for(;;) {
		if (length < COPY_BUFFER_SIZE)
			bytes_read = fread(buffer,1,length,fp);
		else
			bytes_read = fread(buffer,1,COPY_BUFFER_SIZE,fp);
		if (bytes_read <= 0) break;
		fprint_mac_roman_n(output, buffer, bytes_read);
		length -= bytes_read;
	}
}
