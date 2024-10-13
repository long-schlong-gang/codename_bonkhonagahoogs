
#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>

#include "src/ttf_text.h"


static bool redraw = true;


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
	g_TextColours[25] = (SDL_Colour){ 0xFF, 0x80, 0x40, 0xFF };
}


//	Scene Termination
void scn_title_teardown() {
}


//	Scene Event Handler
void scn_title_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;

	if (evt.type == SDL_KEYDOWN) redraw = true;
}


//	Scene Draw Calls
void scn_title_draw_frame() {
	if (!redraw) return;

	SDL_SetRenderDrawColor(g_renderer, 0x40, 0x80, 0xFF, 0xFF);
	SDL_RenderClear(g_renderer);

	TTFText_RenderGlyph(10, 10, TXTCLR_SPECIAL, 0x00FE); // Thorn

	TTFText_RenderText(50, 50, 26, "Héllö, würld!");

	TTFText_Textbox(50, 100, 30, 10, TXTCLR_RESET,
		"'Twas Brillig and the slithy toves did gyre and gymble in the wabe. \n"
		"All mimsy were the borogoves and the mome raths outgrabe."
	);
}


//	Global Scene Handle
Scene scn_title = {
	.setup = &scn_title_setup,
	.teardown = &scn_title_teardown,
	.handle_events = &scn_title_handle_events,
	.draw_frame = &scn_title_draw_frame,
};