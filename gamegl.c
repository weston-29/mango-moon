/* File: gamegl.c
 * ---------- 
 * Weston Keller | CS107E Winter 2025
 * Final Project Version of GL to include sprite drawing and floating-point
 */
#include "gamegl.h"
#include "printf.h"
#include "font.h"
#include "strings.h"
#include "uart.h"
#include "malloc.h"
#include "trig.h"

enum { STARTSTATE, GAMESTATE};

// BITMAPS for Sprites and Effects Animations
static uint8_t zero_deg_pixels[] = { 
	0x03, 0x00, 0x12, 0x00, 0x44, 0x01, 0x10, 0x08, 0x20, 0x20, 0x81, 0x02, 
	0x05, 0x28, 0x13, 0x20, 0x21, 0x00, 0x78, 0x04, 0xD0, 0x0D, 0x20, 0x03, 0x80
};
static uint8_t fifteen_deg_pixels[] = {
	0x00, 0x40, 0x02, 0x80, 0x11, 0x00, 0x84, 0x04, 0x10, 0x20, 0x41, 0x01, 
	0x05, 0x04, 0x4A, 0x20, 0xA1, 0x81, 0x78, 0x01, 0x40, 0x04, 0x80, 0x0C, 0x00
};
static uint8_t thirty_deg_pixels[] = {
	0x00, 0x10, 0x00, 0xA0, 0x0C, 0x80, 0xC2, 0x0C, 0x08, 0x20, 0x41, 0x41, 
	0x15, 0x04, 0x50, 0x20, 0xA1, 0x01, 0xF8, 0x04, 0x80, 0x19, 0x00, 0x18, 0x00
};
static uint8_t fortyfive_deg_pixels[] = {
	0x00, 0x00, 0x00, 0x40, 0x0E, 0x81, 0xC2, 0x08, 0x04, 0x24, 0x14, 0xA0, 
	0x8A, 0x82, 0x18, 0x10, 0x30, 0x81, 0xFC, 0x04, 0x80, 0x05, 0x00, 0x10, 0x00	
};
static uint8_t sixty_deg_pixels[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC7, 0x90, 0xE6, 0x58, 0x25, 0x40, 
	0x9C, 0x04, 0x78, 0x21, 0x73, 0x04, 0xB0, 0x14, 0x00, 0x20, 0x00, 0x00, 0x00
};
static uint8_t seventyfive_deg_pixels[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xC0, 0x10, 0xF0, 0x50, 0x3A, 0x00, 
	0x58, 0x02, 0x70, 0x12, 0xE1, 0x89, 0x78, 0x14, 0x00, 0x20, 0x00, 0x00, 0x00
};
static uint8_t ninety_deg_pixels[] = { 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xC0, 0x88, 0xC1, 0x48, 0xE3, 0x40, 
	0x5C, 0x01, 0x90, 0x0A, 0xA0, 0xCC, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static uint8_t neg_fifteen_deg_pixels[] = {
	0x0C, 0x00, 0x2C, 0x00, 0x88, 0x02, 0x20, 0x08, 0x40, 0x41, 0x01, 0x02, 0x04, 
	0x28, 0x13, 0x60, 0x21, 0x00, 0x7C, 0x00, 0xC8, 0x07, 0x90, 0x23, 0x80
};
static uint8_t neg_thirty_deg_pixels[] = {
	0x18, 0x00, 0x58, 0x01, 0x10, 0x02, 0x20, 0x08, 0x40, 0x21, 0x01, 0x02, 0x04, 
	0x28, 0x11, 0x60, 0x43, 0x00, 0x9F, 0x01, 0xE4, 0x01, 0x48, 0x08, 0xC0
};
static uint8_t neg_fortyfive_deg_pixels[] = { 
	0x00, 0x00, 0xF0, 0x04, 0x30, 0x08, 0x20, 0x20, 0x40, 0x80, 0x81, 0x0A, 
	0x04, 0x2A, 0x11, 0x34, 0x21, 0x90, 0x7E, 0x80, 0x2C, 0x03, 0x00, 0x00, 0x00
};
static uint8_t neg_sixty_deg_pixels[] = {
	0x00, 0x00, 0x00, 0x03, 0xE0, 0x30, 0x70, 0x40, 0x61, 0x80, 0xB1, 0x0B, 
	0x42, 0x29, 0x08, 0x68, 0x1F, 0xC0, 0x1D, 0x00, 0x08, 0x00, 0x20, 0x00, 0x00
};
static uint8_t neg_seventyfive_deg_pixels[] = {
	0x00, 0x00, 0x00, 0x00, 0x60, 0x4E, 0x70, 0x40, 0x4E, 0x00, 0xD4, 0x0A, 
	0x4C, 0x0A, 0x10, 0x70, 0x33, 0x40, 0x38, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00
};
static uint8_t neg_ninety_deg_pixels[] = { 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xE2, 0x30, 0x55, 0x04, 0x98, 0x0A, 
	0xA0, 0x0C, 0x70, 0x28, 0x31, 0x20, 0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static uint8_t thrustup1_pix[] = {
0x10, 0x10, 0x40, 0x41, 0x83, 0x02, 0x08, 0x08, 0x20, 0x20, 0x80, 0xC6, 0x01, 0x10, 0x04, 0x40, 0x1B, 0x00, 0x28, 0x00, 0xA0, 0x01, 0x00, 0x04, 0x00
};
static uint8_t thrustup2_pix[] = {
0x08, 0x21, 0x11, 0x10, 0x44, 0x05, 0x12, 0x02, 0x80, 0x0A, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static uint8_t thrustDiagRt_pix[] = {
0x01, 0x00, 0x00, 0x00, 0x30, 0x00, 0x85, 0x04, 0x70, 0x33, 0x00, 0x90, 0x05, 0x80, 0x18, 0x00, 0xC0, 0x06, 0x00, 0x30, 0x00, 0x80, 0x00, 0x00, 0x00
};
static uint8_t thrustDiagLft_pix[] = {
0x02, 0x00, 0x00, 0x00, 0x30, 0x28, 0x40, 0x38, 0x80, 0x33, 0x00, 0x24, 0x00, 0x68, 0x00, 0x60, 0x00, 0xC0, 0x01, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00
};
static uint8_t thrust90_pix[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x20, 0x20, 0x07, 0x00, 0xF0, 0x54, 0x00, 0x0F, 0x00, 0x01, 0xC1, 0x00, 0x80, 0x00, 0x00, 0x01, 0x00, 0x00
};
static uint8_t thrustNeg90_pix[] = {
0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x40, 0x20, 0xE0, 0x00, 0x78, 0x00, 0x15, 0x07, 0x80, 0xE0, 0x04, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00
};

static uint8_t explode1_pix[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x00, 0x00, 0x4D, 0x01, 0x2C, 0x01, 0x64, 0x01, 0x80, 0x00, 0x40, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static uint8_t explode2_pix[] = {
0x82, 0x00, 0x04, 0x21, 0x01, 0x08, 0xC2, 0x48, 0x49, 0x0C, 0x05, 0x4A, 0x05, 0x28, 0x01, 0x80, 0x08, 0x20, 0x0C, 0x22, 0x01, 0x00, 0x30, 0x00, 0x10
};
static uint8_t explode3_pix[] = {
0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00
};

static uint8_t moon_pixels[] = {
0x00, 0x00, 0x07, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x03, 0xBF, 0xFF, 0xDE, 0x00, 0x00, 0x00, 0x0C, 0xFE, 0x0F, 0xF3, 0x80, 0x00, 0x01, 0x17, 0x80, 0x00, 0xFE, 0xE0, 0x00, 0x00, 0x28, 0x00, 0x00, 0x3F, 0x38, 0x00, 0x00, 0x60, 0x00, 0x00, 0x0F, 0xDC, 0x00, 0x00, 0x84, 0x00, 0x00, 0x03, 0xF6, 0x00, 0x01, 0x80, 0x00, 0x00, 0x03, 0xFB, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0xFD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x80, 0x00, 0x80, 0x00, 0x00, 0x00, 0x7F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C, 0xC0, 0x10, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xD8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xEC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFC, 0x40, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xF4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xEE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xF6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xA2, 0x08, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xF2, 0x40, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x66, 0x40, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE6, 0x08, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xBE, 0x20, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFC, 0x20, 0x00, 0x00, 0x00, 0x00, 0x3E, 0xF4, 0x10, 0x00, 0x00, 0x00, 0x00, 0x3E, 0xF4, 0x38, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xBC, 0x0C, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xF8, 0x16, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xE8, 0x07, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xF8, 0x0F, 0x80, 0x00, 0x00, 0x03, 0xFF, 0xF0, 0x03, 0xC0, 0x00, 0x00, 0x0F, 0xBF, 0xB0, 0x05, 0xF0, 0x00, 0x00, 0x1D, 0xEF, 0xA0, 0x03, 0xFC, 0x00, 0x00, 0x3F, 0xDF, 0xE0, 0x01, 0x7F, 0xC0, 0x01, 0xFF, 0xFF, 0x80, 0x01, 0xFD, 0xFC, 0x0F, 0xFF, 0xFE, 0x80, 0x00, 0xFF, 0xBF, 0xFF, 0xFB, 0xF7, 0x80, 0x00, 0x6F, 0xFF, 0xFF, 0xEF, 0x77, 0x00, 0x00, 0x37, 0xFD, 0xF7, 0xFF, 0xF6, 0x00, 0x00, 0x0F, 0xFF, 0xFC, 0xFF, 0xD8, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0x3F, 0xF0, 0x00, 0x00, 0x01, 0xE5, 0xFF, 0xF9, 0xC0, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0x8F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xCF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00
};

struct {
	uint8_t *zero_deg;
	
	uint8_t *fifteen_deg;
	uint8_t *thirty_deg;
	uint8_t *fortyfive_deg;
	uint8_t *sixty_deg;
	uint8_t *seventyfive_deg;
	uint8_t *ninety_deg;

	uint8_t *neg_fifteen_deg;
	uint8_t *neg_thirty_deg;
	uint8_t *neg_fortyfive_deg;
	uint8_t *neg_sixty_deg;
	uint8_t *neg_seventyfive_deg;
	uint8_t *neg_ninety_deg;

	uint8_t *thrustup1;
	uint8_t *thrustup2;
	uint8_t *thrustDiagRt;
	uint8_t *thrustDiagLft;
	uint8_t *thrust90;
	uint8_t *thrustNeg90;
	
	uint8_t *explode1;
	uint8_t *explode2;
	uint8_t *explode3;
} cache;


void gamegl_init(int width, int height, gl_mode_t mode) {
    fb_init(width, height, mode);
}

int gamegl_get_width(void) {
    return fb_get_width();
}

int gamegl_get_height(void) {
    return fb_get_height();
}

color_t gamegl_color(uint8_t r, uint8_t g, uint8_t b) {
    color_t result = 0xff000000; // alpha bits always ff
	result ^= r << 16;
	result ^= g << 8;
	result ^= b;
	return result;
}

void gamegl_swap_buffer(void) {
    return fb_swap_buffer();
}

void gamegl_clear(color_t c) { 
    unsigned int *ptr = fb_get_draw_buffer(); // make type 32 bit to write color all at once
 	int width = gamegl_get_width();
	int height = gamegl_get_height();
	int npix = width * height;
	
	int divisibility = 1;

	divisibility = (npix % 2 == 0) ? 2 : divisibility;
	divisibility = (npix % 4 == 0) ? 4 : divisibility;
	divisibility = (npix % 8 == 0) ? 8 : divisibility;

	// Unrolled loop for efficiency
	switch (divisibility) {
		case 8:
			for (int i = 0; i < npix; i+=8) {
				ptr[i] = c; 
				ptr[i+1] = c;
				ptr[i+2] = c;	
        		ptr[i+3] = c; 
        		ptr[i+4] = c; 
				ptr[i+5] = c;
				ptr[i+6] = c;
				ptr[i+7] = c;
			}
			break;

		case 4:
			for (int i = 0; i < npix; i+=4) {
        		ptr[i] = c; 
				ptr[i+1] = c;
				ptr[i+2] = c;
				ptr[i+3] = c;
    		}
			break;

		case 2:
			for (int i = 0; i < npix; i+=2) {
        		ptr[i] = c; 
				ptr[i+1] = c;
    		}
			break;
		default:
			for (int i = 0; i < npix; i++) {
        		ptr[i] = c; 
    		}
			break;
	}
}

// SECOND clear function that takes parameter to only clear up to that point
void gamegl_clear_height(color_t c, int height) { 
    unsigned int *ptr = fb_get_draw_buffer(); // make type 32 bit to write color all at once
 	int width = gamegl_get_width();
	// int height = gamegl_get_height();
	int npix = width * height;
	
	int divisibility = 1;

	divisibility = (npix % 2 == 0) ? 2 : divisibility;
	divisibility = (npix % 4 == 0) ? 4 : divisibility;
	divisibility = (npix % 8 == 0) ? 8 : divisibility;

	// Unrolled loop for efficiency
	switch (divisibility) {
		case 8:
			for (int i = 0; i < npix; i+=8) {
				ptr[i] = c; 
				ptr[i+1] = c;
				ptr[i+2] = c;	
        		ptr[i+3] = c; 
        		ptr[i+4] = c; 
				ptr[i+5] = c;
				ptr[i+6] = c;
				ptr[i+7] = c;
			}
			break;

		case 4:
			for (int i = 0; i < npix; i+=4) {
        		ptr[i] = c; 
				ptr[i+1] = c;
				ptr[i+2] = c;
				ptr[i+3] = c;
    		}
			break;

		case 2:
			for (int i = 0; i < npix; i+=2) {
        		ptr[i] = c; 
				ptr[i+1] = c;
    		}
			break;
		default:
			for (int i = 0; i < npix; i++) {
        		ptr[i] = c; 
    		}
			break;
	}
}

void gamegl_draw_pixel(int x, int y, color_t c) {
	unsigned int pixinrow = gamegl_get_width();
	unsigned int (*grid)[pixinrow] = fb_get_draw_buffer();
	if (x < gamegl_get_width() && y < gamegl_get_height() && x >= 0 && y >= 0) { // make sure coordinate in bounds - STRICTLY less than
		grid[y][x] = c;
	}
}

color_t gamegl_read_pixel(int x, int y) {
	unsigned int pixinrow = gamegl_get_width();
	unsigned int pixincol = gamegl_get_height();
	unsigned int (*grid)[pixinrow] = fb_get_draw_buffer();
	if (x >= pixinrow || y >= pixincol || x < 0 || y < 0) {
		return 0;
	} 
	color_t coloratloc = grid[y][x];
	return coloratloc;
}

void gamegl_draw_rect(int x, int y, int w, int h, color_t c) {
	int x_max = (x + w) > gamegl_get_width() ? gamegl_get_width() : (x + w); // CLIPPING: off right of screen
	int y_max = (y + h) > gamegl_get_height() ? gamegl_get_height() : (y + h); // off bottom
	int x_min = (x <= 0) ? 0 : x; // off left of screen
	int y_min = (y <= 0) ? 0 : y; // off top of screen
	
	// add bounds-checking intersection for requested width/ht
	for (int i = x_min; i < x_max; i++) {
		for (int j = y_min; j < y_max; j++) {
			gamegl_draw_pixel(i, j, c);
		}
	}
}

void gamegl_draw_char(int x, int y, char ch, color_t c) {
	int charwidth = font_get_glyph_width();
	int charheight = font_get_glyph_height();
	int fbwidth = gamegl_get_width();
	int fbheight = gamegl_get_height();

	uint8_t buf[font_get_glyph_size()];
	if (!font_get_glyph(ch, buf, sizeof(buf))) { // wrapped this in IF to prevent invalid char requests within font )
		return;
	}

	uint8_t (*img)[charwidth];
	img = &buf; // point at the same thing buf is pointing to
	
	int x_max = ((x + charwidth) >= fbwidth) ? (fbwidth - x) : charwidth;
	int y_max = ((y + charheight) >= fbheight) ? (fbheight - y) : charheight;
	
	for (int j = 0; j < y_max; j++) {
		for (int i = 0; i < x_max; i++) {
			if (img[j][i] == 0xff) {
				gamegl_draw_pixel(x + i, y + j, c);
			}
		}
	}
}

void gamegl_draw_moon(int x, int y, color_t c) {
	int moonwidth = 56;
	int moonheight = 56;
		
	uint8_t *buf = malloc(moonwidth * moonheight);
	uint8_t *pixeldata = (void*)&moon_pixels;

	// Read bitmap into cache buffer
	int index = 0;
	for (int y = 0; y < moonheight; y++) {
		for (int x = 0; x < moonwidth; x++) {
			int bit_index = (y * moonwidth) + x; 
			int bit_start = bit_index / 8;
			int bit_offset = bit_index % 8;
			// get one pixel from relevant rotation bitmap
			int val = pixeldata[bit_start] & (1 << (7 - bit_offset)); 
			// read into buf depending if 0xFF or 0x00
			buf[index++] = val != 0 ? 0xFF : 0x00;	
		}
	}
	
	// Draw moon onscreen
	for (int j = 0; j < moonheight; j++) {
		for (int i = 0; i < moonwidth; i++) {
			if (buf[(j * moonwidth) + i] == 0xff) {
				gamegl_draw_pixel(x + i, y + j, c);
			}
		}
	}
	free(buf);	
}

void write_bitmap(int rotation, uint8_t *buf, int thrust, int explode) {
	
	uint8_t *pixeldata;
	
	// Select array of pixel bitmap data for relevant rotation
	if (!thrust && !explode) {
		switch (rotation) {
			case 0:
   				pixeldata = (void*)&zero_deg_pixels;
				break;
			case 15:
   				pixeldata = (void*)&fifteen_deg_pixels;
				break;
			case 30:
   				pixeldata = (void*)&thirty_deg_pixels;
				break;
			case 45:
				pixeldata = (void*)&fortyfive_deg_pixels;
				break;
			case 60:
   				pixeldata = (void*)&sixty_deg_pixels;
				break;
			case 75:
   				pixeldata = (void*)&seventyfive_deg_pixels;
				break;
			case 90:
				pixeldata = (void*)&ninety_deg_pixels;
				break;
			case -15:
   				pixeldata = (void*)&neg_fifteen_deg_pixels;
				break;
			case -30:
   				pixeldata = (void*)&neg_thirty_deg_pixels;
				break;
			case -45:
				pixeldata = (void*)&neg_fortyfive_deg_pixels;
				break;
			case -60:
   				pixeldata = (void*)&neg_sixty_deg_pixels;
				break;
			case -75:
	   			pixeldata = (void*)&neg_seventyfive_deg_pixels;
				break;
			case -90:
				pixeldata = (void*)&neg_ninety_deg_pixels;
				break;
			default: return;
		}
	} 
	else if (thrust) { // for thrusting anim
		if (rotation == 0) {
			pixeldata = (void*)&thrustup1_pix;
		} else if (rotation == 90) {
			pixeldata = (void*)&thrust90_pix;
		} else if (rotation == -90) {
			pixeldata = (void*)&thrustNeg90_pix;
		} else if (rotation > 0) {
			pixeldata = (void*)&thrustDiagRt_pix;
		} else {
			pixeldata = (void*)&thrustDiagLft_pix;
		}
	} else { // pixels for exploding animation, argument 1-3 depending on frame
		if (explode == 1) {
			pixeldata = (void*)&explode1_pix;
		}
		else if (explode == 2) {
			pixeldata = (void*)&explode2_pix;
		} else {
			pixeldata = (void*)&explode3_pix;
		}
	}

	// Read bitmap into cache buffer
	int index = 0;
	for (int y = 0; y < 14; y++) {
		for (int x = 0; x < 14; x++) {
			int bit_index = (y * 14) + x; 
			int bit_start = bit_index / 8;
			int bit_offset = bit_index % 8;
			// get one pixel from relevant rotation bitmap
			int val = pixeldata[bit_start] & (1 << (7 - bit_offset)); 
			// read into buf depending if 0xFF or 0x00
			buf[index++] = val != 0 ? 0xFF : 0x00;	
		}
	}
}

void gamegl_draw_lander(int x, int y, int rotation, color_t c) {
	int spritewidth = 14;
	int spriteheight = 14;
	int fbwidth = gamegl_get_width();
	int fbheight = gamegl_get_height();

	if ((rotation % 15) || ((rotation % 360) > 90 && (rotation % 360) < 270)) {
		return; // invalid angle passed
	}
	
	// Create pointer to refer to cache buffer that stores bitmap data
	uint8_t **bufptr;

	// Select relevant bitmap cache
	switch (rotation) {
		case 0:
			bufptr = &cache.zero_deg;
			break;
		case 15:
			bufptr = &cache.fifteen_deg;
			break;
		case 30:
			bufptr = &cache.thirty_deg;
			break;
		case 45:
			bufptr = &cache.fortyfive_deg;
			break;
		case 60:
			bufptr = &cache.sixty_deg;
			break;
		case 75:
			bufptr = &cache.seventyfive_deg;
			break;
		case 90: 
			bufptr = &cache.ninety_deg;
			break;
		case -15:
			bufptr = &cache.neg_fifteen_deg;
			break;
		case -30:
			bufptr = &cache.neg_thirty_deg;
			break;
		case -45:
			bufptr = &cache.neg_fortyfive_deg;
			break;
		case -60:
			bufptr = &cache.neg_sixty_deg;
			break;
		case -75:
			bufptr = &cache.neg_seventyfive_deg;
			break;
		case -90:
			bufptr = &cache.neg_ninety_deg;
			break;
		default: return; // invalid angle passed
	}

	// Check if bitmap for current rotation has already been initialized
	if (*bufptr == NULL) {
		*bufptr = malloc(spritewidth * spriteheight);
		write_bitmap(rotation, *bufptr, 0, 0);
	} 

	// Read buf data
	uint8_t *buf = *bufptr; // point at the same thing buf is pointing to

	// Clip sprites when drawn on edge of screen for efficiency
	int x_max = ((x + spritewidth) >= fbwidth) ? (fbwidth - x) : spritewidth;
	int y_max = ((y + spriteheight) >= fbheight) ? (fbheight - y) : spriteheight;
	
	// Draw sprite onscreen
	for (int j = 0; j < y_max; j++) {
		for (int i = 0; i < x_max; i++) {
			if (buf[(j * spritewidth + i)] == 0xff) {
				gamegl_draw_pixel(x + i, y + j, c);
			}
		}
	}
	// free(buf);
}

void gamegl_draw_effect(int x, int y, int rotation, int thrust, int explode, color_t c) {
	int spritewidth = 14;
	int spriteheight = 14;
	int fbwidth = gamegl_get_width();
	int fbheight = gamegl_get_height();

	if ((rotation % 15) || ((rotation % 360) > 90 && (rotation % 360) < 270)) {
		return; // invalid angle passed
	}
	
	// Create pointer to refer to cache buffer that stores bitmap data
	uint8_t **bufptr;

	// Select relevant bitmap cache
	if (thrust) {
		if (rotation == 0) {
			bufptr = &cache.thrustup1;
		}
		else if (rotation == 90) {
			bufptr = &cache.thrust90;
		}
		else if (rotation == -90) {
			bufptr = &cache.thrustNeg90;
		}
		else if (rotation > 0) {
			bufptr = &cache.thrustDiagRt;
		} else {
			bufptr = &cache.thrustDiagLft;
		}
	} else {
		if (explode == 1) {
			bufptr = &cache.explode1;
		}
		else if (explode == 2) {
			bufptr = &cache.explode2;
		} else {
			bufptr = &cache.explode3;
		}
	}

	// Check if bitmap for current rotation has already been initialized
	if (*bufptr == NULL) {
		*bufptr = malloc(spritewidth * spriteheight);
		write_bitmap(rotation, *bufptr, thrust, explode);
	} 

	uint8_t *buf = *bufptr;

	// Clip sprites when drawn on edge of screen for efficiency
	int x_max = ((x + spritewidth) >= fbwidth) ? (fbwidth - x) : spritewidth;
	int y_max = ((y + spriteheight) >= fbheight) ? (fbheight - y) : spriteheight;
	
	// Draw sprite onscreen
	for (int j = 0; j < y_max; j++) {
		for (int i = 0; i < x_max; i++) {
			if (buf[(j * spritewidth) + i] == 0xff) {
				gamegl_draw_pixel(x + i, y + j, c);
			}
		}
	}
	// free(buf);
}

void swap(int *a, int *b) {
   int temp = *a;
   *a = *b;
   *b = temp;
}

// Xialoin Wu's line algorithm that allows for slopes greater than 1 (translated from C++)
// https://www.geeksforgeeks.org/anti-aliased-line-xiaolin-wus-algorithm/
void gamegl_draw_line(int x0, int y0, int x1, int y1, color_t c) {
   // need to check if slope is greater than 1
   int steep = fabs(y1 - y0) > fabs(x1 - x0);
   // if slope is greater than 1, swap the x and y's
   if (steep) {
      swap(&x0, &y0);
      swap(&x1, &y1);
   }

   // if drawing from right to left, swap the x0 with x1, y0 with y1 to draw left to right
   if (x0 > x1) {
      swap(&x0, &x1);
      swap(&y0, &y1);
   }  

   // computes the slope (gradient)
   float dx = x1 - x0;
   float dy = y1 - y0;
   float gradient = dy/dx;
   if (dx == 0.0f) {
      gradient = 1;
   }
   float intersectY = y0;

   // draws line with anti aliasing
   for (int x = x0; x <= x1; x++) {
       // returns integer part of floating point y0
       int y = (int)intersectY;
       // returns fractional part of floating point y0
       float frac = intersectY - y;
       if (intersectY < 0) {
       frac = intersectY - (y + 1); // 'fPartofNumber' case handling negative y intersect
       }
   
       // determine alpha (transparency) based on proximity to line
       uint8_t alpha_main = (uint8_t)((1.0 - fabs(frac)) * 255); //1.0f
       uint8_t alpha_neighbor = (uint8_t)(fabs(frac) * 255);
       int y_diff = 0;
   
       // if slope is greater than 1, switch the x and y values when drawing
       if (steep) {
           gamegl_draw_pixel(y, x, c | (alpha_main << 24));
           if (frac > 0) {
              y_diff = y + 1;
       }
       else {
              y_diff = y - 1;
       }
           gamegl_draw_pixel(y_diff, x, c | (alpha_neighbor << 24));
       } else {
           gamegl_draw_pixel(x, y, c | (alpha_main << 24));
           if (frac > 0) {
              y_diff = y + 1;
       }
       else {
              y_diff = y - 1;
       }
           gamegl_draw_pixel(x, y_diff, c | (alpha_neighbor << 24));
       }
   
       intersectY += gradient;
   }
}


void gamegl_draw_string(int x, int y, const char* str, color_t c) {
	int charwidth = font_get_glyph_width();	
	int numchars = (int)strlen(str);
	int strwidth = numchars * charwidth;
	int fbwidth = fb_get_width();
	int clippedwidth = (strwidth >= fbwidth) ? fbwidth : strwidth;
	
	char *cur = (char*)str; // point to same memory as str

	for (int i = 0; i < clippedwidth; i += charwidth) {
		if (*cur == '\0') {
			break;
		}
		gamegl_draw_char(x + i, y, *cur, c); 
		cur++; // go to next char
	}
}

// draws starry background onscreen
void gamegl_draw_stars(int state) {
	int startStars[][2] = {
		{25, 340}, {70, 100}, {125, 175}, {200, 260}, {220, 245}, {210, 270}, {250, 115}, {370, 202},
		{512, 349}, {632, 431}, {670, 369}, {674, 372}, {740, 194}, {730, 510}, {710, 610}, {760, 659}, {710, 589}, {723, 530},
		// Big Dipper
		{456, 187}, {498, 248}, {550, 218}, {555, 158}, {585, 110}, {628, 81}, {690, 75},
		// Orion's Belt
		{15, 218}, {85, 365}, {175, 490}, {175, 491}, {174, 490}, {174, 491}, {155, 590}, {174, 491}, {195, 486}, {195, 485}, {196, 486}, {196, 485}, {218, 467}, {218, 466}, {219, 467}, {219, 466}, {258, 548}, {218, 467}, {205, 315}, {295, 285}, {205, 315}, {125, 240}, {85, 365} 
	};
	
	int gameStars[][2] = {	
		{25, 340}, {70, 100}, {125, 175}, {200, 260}, {220, 245}, {210, 270}, {250, 115}, {370, 202},
		{512, 349}, {632, 431}, {670, 369}, {674, 372}, {740, 194}, {730, 510}, {710, 610}, {760, 659},
		// Big Dipper
		{456, 187}, {498, 248}, {550, 218}, {555, 158}, {585, 110}, {628, 81}, {690, 75},
		// Orion's Belt
		{15, 218}, {85, 365}, {174, 491}, {218, 467}, {218, 466}, {219, 467}, {219, 466},{218, 467}, {205, 315}, {295, 285}, {205, 315}, {125, 240}, {85, 365} 
	};


	int numStars = (state == STARTSTATE) ? (sizeof(startStars) / sizeof(startStars[0])) : (sizeof(gameStars) / sizeof(gameStars[0]));
	for (int i = 0; i < numStars; i++) {
		if (state == STARTSTATE) {
			gamegl_draw_pixel(startStars[i][0], startStars[i][1], GL_SILVER);
		} else {
			gamegl_draw_pixel(gameStars[i][0], gameStars[i][1], GL_SILVER);
		}
	}	

	// Mars :D
	gamegl_draw_pixel(321, 104, GL_CAYENNE);
	gamegl_draw_pixel(321, 103, GL_CAYENNE);
	gamegl_draw_pixel(322, 104, GL_CAYENNE);
	gamegl_draw_pixel(322, 103, GL_CAYENNE);
}

// Connect Stars in Constellations
void gamegl_draw_constellations(void) {
	int dipper[][2] = {
		// Big Dipper
		// {456, 187}, {498, 248}, {550, 218}, {555, 158}, {456, 187}, {555, 158},  {585, 110}, {628, 81}, {690, 75}
		{456, 187}, {498, 248}, {550, 218}, {555, 158}, {585, 110}, {628, 81}, {690, 75}
	};

	int orion[][2] = {
		// Orion's Belt
		{15, 218}, {85, 365}, {175, 490}, {175, 491}, {174, 490}, {174, 491}, {155, 590}, {174, 491}, {195, 486}, {218, 467}, {258, 548}, {218, 467}, {205, 315}, {295, 285}, {205, 315}, {125, 240}, {85, 365} 
	};

	int dipperSize = sizeof(dipper) / sizeof(dipper[0]);
	for (int i = 0; i < dipperSize - 1; i++) {
                gamegl_draw_line(dipper[i][0], dipper[i][1], dipper[i + 1][0], dipper[i + 1][1], 0xff121212);
	}
	int orionSize = sizeof(orion) / sizeof(orion[0]);
	for (int i = 0; i < orionSize - 1; i++) {
    	// Uncomment below to connect constellations between Orion's Stars
		//         gamegl_draw_line(orion[i][0], orion[i][1], orion[i + 1][0], orion[i + 1][1], 0xff121212);
	}
}

// draws mountains by creating a connect the dot image and drawing lines between all the points
// takes parameter state that draws basic mountains, or zooms in  
void gamegl_draw_mountains(int state) {
        int screenWidth = gamegl_get_width();
	int screenHeight = gamegl_get_height();

	int startIndex;
	int endIndex;
	double scale = 1;
        int centerX;
	int centerY;
    
        double offsetX = 0.0; 
	double offsetY = 300.0;
        
        switch(state) {
	    // base mountain range
            case 0: 
                scale = 1;
                offsetX = 0.0;
                offsetY = 300.0;
                break;
	    case 1:
		scale = 3.0;
		offsetX = 0.0;
	        offsetY = -100.0;
		break;
            case 2:
                // zoom in on second landing zone
                scale = 3.0;
                offsetX =  0.0; 
                offsetY = -100.0;
                break;
            case 3:
                // zoom in on third landing zone
                scale = 3.0;
                offsetX = -500.0;
                offsetY = -100.0;
                break;
            case 4:
                // zoom in on fourth landing zone
                scale = 3.0;
                offsetX = -1375.0;
                offsetY = -110.0;
                break;
	    case 5:
		// zoom in on fifth landing zone
		scale = 3.0;
		offsetX = -1600.0;
		offsetY = -110.0;
		break;
            default:
                // default to normal mountains if an unknown state is given
                scale = 1.0;
                offsetX = 0.0;
                offsetY = 300.0;
                break;
        }





        int points[][2] = {
	    // first choppy mountains
            {0, 225}, {10, 215}, {20, 205}, {30, 210}, {40, 200},
            {50, 190}, {60, 195}, {70, 205}, {80, 215}, {90, 225},
            {100, 215}, {110, 210}, {120, 205}, {130, 200}, {140, 195},
            {150, 225},
            // center mountain
            {160, 215}, {170, 210}, {175, 200}, {182, 190}, {190, 175},
            {195, 150}, {200, 140}, {205, 145}, {210, 135}, {215, 130},
            {220, 120}, {225, 115}, {232, 110}, {240, 115}, {250, 125},
            {255, 140}, {260, 155}, {267, 175}, {275, 190}, {285, 210},
            {295, 225},
            // right mountains
            {305, 215}, {310, 210}, {315, 205}, {320, 200}, {325, 195},
            {330, 200}, {335, 210}, {340, 215}, {345, 210}, {350, 205},
            {355, 200}, {360, 190}, {365, 185}, {370, 190}, {375, 195},
            {380, 190}, {385, 185}, {390, 180}, {395, 175}, {400, 170},
            // mountains from 400-800
	    {400, 170}, {407, 165}, {415, 170}, {420, 165}, {425, 170},
            {430, 180}, {440, 195}, {445, 200}, {460, 190}, {475, 205},
            {490, 220}, {500, 235}, {510, 225}, 
	    {520, 215}, {525, 210}, {530, 220}, {535, 225}, {540, 205},
	    {555, 180}, {560, 182}, {565, 190},  
            {570, 190}, {575, 200}, {580, 195}, {585, 205}, {595, 198}, 
	    {600, 189}, {605, 200}, {610, 210}, {615, 200}, {620, 195},
            {625, 205}, {640, 215}, {650, 209}, {655, 200}, {660, 190},
	    {670, 185}, {675, 179}, {680, 174}, 
            {685, 170}, {690, 152}, {695, 145}, {700, 152}, {705, 160}, 
	    {710, 162}, {715, 175}, {730, 190}, {735, 199}, {740, 196},  
            {745, 205}, {760, 195}, {775, 225}, {790, 210}, {800, 250}
	};

	// explicitly change two neighboring y values to be the same to create flat landing zones
	// {60,195} -> {70,195}
        points[6][1] = 195;
        points[7][1] = 195;
        // {200,140} ->{205,140} 5X
        points[21][1] = 140;
        points[22][1] = 140;
        // {330,200} -> {335,200}
        points[35][1] = 200;
        points[36][1] = 200;

        // {605, 205} -> {610, 205} -> {615, 200}
	points[84][1] = 205;
	points[85][1] = 205;
        points[86][1] = 205;

	// {730, 190} -> {735, 190} -> {740, 196}
	points[103][1] = 203;
	points[104][1] = 203;
        points[105][1] = 203;

        color_t color = GL_WHITE;	
	// draw line between each point consecutively
	int numPoints = sizeof(points)/sizeof(points[0]);
	for (int i = 0; i < numPoints - 1; i++) {
	   if ((i == 6 || i == 21 || i == 35 || i == 84 || i == 85 || i == 103 || i == 104) && state != 0) {
              color = GL_GREEN;
	   }
	   else {
              color = GL_WHITE;
	   }
	   double x1 = (points[i][0] + 0) * scale + offsetX;
	   double y1 = (points[i][1] + 0) * scale + offsetY;
	   double x2 = (points[i + 1][0] + 0) * scale + offsetX;
	   double y2 = (points[i + 1][1] + 0) * scale + offsetY;
           gamegl_draw_line(x1, y1, x2, y2, color);
        }

	int zone1x = 50 * scale + offsetX;
	int zone1y = 210 * scale + offsetY;
	int zone2x = 200 * scale + offsetX;
	int zone2y = 160 * scale + offsetY;
	int zone3x = 280 * scale + offsetX;
	int zone3y = 220 * scale + offsetY;
	int zone4x = 600 * scale + offsetX;
	int zone4y = 210 * scale + offsetY;
	int zone5x = 725 * scale + offsetX;
        int zone5y = 210 * scale + offsetY;	
	gamegl_draw_string(zone1x, zone1y, "2x", GL_WHITE);
	gamegl_draw_string(zone2x, zone2y, "5x", GL_WHITE);
        gamegl_draw_string(zone3x, zone3y, "3x", GL_WHITE);
        gamegl_draw_string(zone4x, zone4y, "2x", GL_WHITE);
        gamegl_draw_string(zone5x, zone5y, "5x", GL_WHITE);
        // gamegl_draw_string(zone5x, zone5y, "3x", GL_WHITE);

}	

int gamegl_get_char_height(void) {
	return font_get_glyph_height();
}

int gamegl_get_char_width(void) {
	return font_get_glyph_width();
}

