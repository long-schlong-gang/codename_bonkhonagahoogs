
#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>

#include "src/ttf_text.h"


static bool redraw = true;

static bool show_box = false;
static Uint64 box_tick = 0;
static TTFText_Box box = {
	50, 100, 30, 10, CLR_RESET, 0,
	"'Twas Brillig and the slithy toves did gyre and gymble in the wabe. \n"
	"All mimsy were the borogoves and the mome raths outgrabe."
};


//	Menu Option Callbacks
void scn_title_cb_play() {
}

void scn_title_cb_hello() {
	puts("Hello, world!");
	fflush(stdout);
}

void scn_title_cb_exit() {
	g_isRunning = false;
}


//	Scene Initialisation
void scn_title_setup() {
}


//	Scene Termination
void scn_title_teardown() {
}


//	Scene Event Handler
void scn_title_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;

	// DEBUG: Get typed-out text box working
	Uint64 now = SDL_GetTicks64();

	if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_F1) {
		if (show_box) {
			show_box = false;
		} else {
			show_box = true;
			box_tick = now;
			box.charcount = 1;
		}
	}

	if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_SPACE) {
		if (show_box && box.charcount > 0) {
			box.charcount = -1;
		}
	}
	
	if (box.charcount > -1) {
		box.charcount = (now - box_tick) / 50;
	}

	if (box.charcount > box.cols * box.rows) {
		box.charcount = -1;
	}

	redraw = true;
}


//	Scene Draw Calls
void scn_title_draw_frame() {
	if (!redraw) return;

	Colours_SetRenderer(CLR_WINDOW_BG);
	SDL_RenderClear(g_renderer);

	TTFText_RenderGlyph(10, 10, CLR_SPECIAL, 0x00FE); // Thorn
	TTFText_RenderText(50, 50, 26, "Héllö, würld!");

	if (show_box) TTFText_Draw_Box(box);
}


//	Global Scene Handle
Scene scn_title = {
	.setup = &scn_title_setup,
	.teardown = &scn_title_teardown,
	.handle_events = &scn_title_handle_events,
	.draw_frame = &scn_title_draw_frame,
};