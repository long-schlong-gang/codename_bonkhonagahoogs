
#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>

#include "src/menu_element.h"
#include "src/ttf_text.h"
#include "src/dialogue.h"
#include "src/sound.h"
#include "src/world.h"
#include "src/pix.h"


//	Scene Initialisation
void scn_world_setup() {
}


//	Scene Termination
void scn_world_teardown() {
}


//	Scene Event Handler
void scn_world_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;

	World_HandleEvents(evt);

	if (evt.type == SDL_KEYUP && evt.key.keysym.sym == SDLK_ESCAPE) {
		Scene_Set("title");
	}
}


//	Scene Draw Calls
void scn_world_draw_frame() {
	Colours_SetRenderer(CLR_WINDOW_BG);
	SDL_RenderClear(g_renderer);

	World_Draw();
}


//	Global Scene Handle
Scene scn_world = {
	.setup = &scn_world_setup,
	.teardown = &scn_world_teardown,
	.handle_events = &scn_world_handle_events,
	.draw_frame = &scn_world_draw_frame,
};