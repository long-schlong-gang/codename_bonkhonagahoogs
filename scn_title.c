
#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>
#include <menu.h>


static Menu *__title = NULL;


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
	__title = Menu_Create(NULL);
	Menu_AddOption(__title, "Play Game", &scn_title_cb_play);
	Menu_AddOption(__title, "Say Hello", &scn_title_cb_hello);
	Menu_AddOption(__title, "Exit", &scn_title_cb_exit);
}


//	Scene Termination
void scn_title_teardown() {
	Menu_Destroy(__title);
}


//	Scene Event Handler
void scn_title_handle_events(SDL_Event evt) {
	Menu_HandleInput(__title, evt);
}


//	Scene Draw Calls
void scn_title_draw_frame() {
	SDL_SetRenderDrawColor(g_renderer, 0x2c, 0x8d, 0xa3, 0xFF);
	SDL_RenderClear(g_renderer);

	Menu_Draw(__title, 10, 10);
}


//	Global Scene Handle
Scene scn_title = {
	.setup = &scn_title_setup,
	.teardown = &scn_title_teardown,
	.handle_events = &scn_title_handle_events,
	.draw_frame = &scn_title_draw_frame,
};