/* File: fb.c
 * ----------
 * The framebuffer module abstracts the functions of initializing a 
 * single or double-buffered framebuffer routine to display to a screen.
 * A graphics library can use these routines to initialize a framebuffer 
 * of specified screen dimensions and single/doublebuffer status,
 * and swap between front and back buffers as necessary., 
 */
#include "fb.h"
#include "de.h"
#include "hdmi.h"
#include "malloc.h"
#include "strings.h"

enum { BUF1, BUF2 };

// module-level variables, you may add/change this struct as you see fit
static struct {
    int width;             // count of horizontal pixels
    int height;            // count of vertical pixels
    int depth;             // num bytes per pixel
	void *framebuf1;       // address of 'front buffer' memory - static variables in struct
						      // are allocated in the data section, and will be init'd to NULL
	void *framebuf2;
	int activebuf;
	fb_mode_t mode;
} module;

void fb_init(int width, int height, fb_mode_t mode) {
	if (module.framebuf1 != NULL) { free(module.framebuf1); } // check if reinitialization
	if (module.framebuf2 != NULL) { free(module.framebuf2); }

    module.width = width;
    module.height = height;
    module.depth = 4;
    int nbytes = module.width * module.height * module.depth;
    module.framebuf1 = malloc(nbytes);
    memset(module.framebuf1, 0x0, nbytes);
	
	if (mode == FB_DOUBLEBUFFER) {
		module.mode = FB_DOUBLEBUFFER;
		module.framebuf2 = malloc(nbytes);
		memset(module.framebuf2, 0x0, nbytes); // initialize back buffer to all black
		module.activebuf = BUF1; // initialize framebuf1 to be active/front buffer, as set below
	} else {
		module.mode = FB_SINGLEBUFFER;
	}

    hdmi_resolution_id_t id = hdmi_best_match(width, height);
    hdmi_init(id);
    de_init(width, height, hdmi_get_screen_width(), hdmi_get_screen_height());
    de_set_active_framebuffer(module.framebuf1);
}

int fb_get_width(void) {
    return module.width;
}

int fb_get_height(void) {
    return module.height;
}

int fb_get_depth(void) {
    return module.depth; // always 4 in our 32-bit pixel BGRA system
}

void* fb_get_draw_buffer(void){
	if (module.mode == FB_SINGLEBUFFER) {
		return module.framebuf1; // only one framebuffer in use
	}
	void *drawbuf = (module.activebuf == BUF1) ? module.framebuf2 : module.framebuf1; // return inactive buffer
	return drawbuf;
}

void fb_swap_buffer(void) {
	if (module.mode == FB_DOUBLEBUFFER) { // if mode is singlebuffer, no-op
		void *inactivebuffer = fb_get_draw_buffer();
		module.activebuf = (module.activebuf == BUF1) ? BUF2 : BUF1; // swap which framebuffer is marked as active
		de_set_active_framebuffer(inactivebuffer);
	}
	return;
}
