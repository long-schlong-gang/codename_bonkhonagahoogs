
#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>

#include "src/menu_element.h"
#include "src/ttf_text.h"
#include "src/dialogue.h"
#include "src/sound.h"
#include "src/pix.h"
#include "src/gamestate.h"


//	Scene Initialisation
void scn_dialogue_setup() {
	if (g_CurrentDialogue.bg_music != OST_NONE) {
		Sound_OST_QueueTrack(g_CurrentDialogue.bg_music);
		Sound_OST_FadeNext(1000);
	}

	Dialogue_Start();
}


//	Scene Termination
void scn_dialogue_teardown() {
	Dialogue_UnloadTree();

	Sound_OST_ClearQueue();
	Sound_OST_FadeNext(250);
}


//	Scene Event Handler
void scn_dialogue_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;

	Dialogue_HandleEvents(evt);

	if (g_CurrentDialogue.current == NULL) {
		if (Gamestate_GetFlag(GFLAG_BAD_END) > 0) {
			g_isRunning = false;
			return;
		}

		Gamestate_NextScene();
	}
}


//	Scene Draw Calls
void scn_dialogue_draw_frame() {
	Colours_SetRenderer(CLR_WINDOW_BG);
	SDL_RenderClear(g_renderer);

	Dialogue_DrawAll();
}


//	Global Scene Handle
Scene scn_dialogue = {
	.setup = &scn_dialogue_setup,
	.teardown = &scn_dialogue_teardown,
	.handle_events = &scn_dialogue_handle_events,
	.draw_frame = &scn_dialogue_draw_frame,
};