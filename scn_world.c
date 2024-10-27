
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
	Sound_SFX_Prepare(SFX_DIT_UP);
	Sound_SFX_Prepare(SFX_STEP_1);
	Sound_SFX_Prepare(SFX_STEP_2);

	Sound_OST_QueueTrack(OST_LOUNGE);
	Sound_OST_FadeNext(1000);

	// Check for cutscenes
	switch (Gamestate_GetFlag(GFLAG_CUTSCENE)) {
		case CUTSCENE_NUM_NONE: break;
		case CUTSCENE_NUM_INTRO: {
			Dialogue_LoadTree(DIALOGUE_INTRO);
			g_CurrentDialogue.background = PIX_BG_SPACE;
			Pix_Load(PIX_BG_SPACE);

			// Set up initial state of the world
			Gamestate_SetFlag(GFLAG_WORLD_ROOM, ROOM_QUART_CAPTAIN);
			Gamestate_SetFlag(GFLAG_WORLD_POS_X, 2);
			Gamestate_SetFlag(GFLAG_WORLD_POS_Y, 2);
			Gamestate_SetFlag(GFLAG_WORLD_DIR, DIR_DOWN);

			Gamestate_SetFlag(GFLAG_ERUYA_DIA, 01);

			Gamestate_SetFlag(GFLAG_SCENE_NUM, SCENE_NUM_WORLD);
			Gamestate_SetFlag(GFLAG_ACT_NUM, ACT_NUM_TURBULENCE);
			Gamestate_SetFlag(GFLAG_CUTSCENE, CUTSCENE_NUM_NONE);
			Scene_Set("dia");
		} break;
		case CUTSCENE_NUM_OUTTRO: {
			Log_Message(LOG_DEBUG, "Triggered outtro");
			Dialogue_LoadTree("assets/txt/outtro_cutscene.dbf");
			g_CurrentDialogue.background = PIX_BG_SPACE;
			Pix_Load(PIX_BG_SPACE);

			Gamestate_SetFlag(GFLAG_END, 1);
			Gamestate_SetFlag(GFLAG_CUTSCENE, CUTSCENE_NUM_NONE);
			Scene_Set("dia");
		} break;
	}

	World_Teleport(
		Gamestate_GetFlag(GFLAG_WORLD_ROOM),
		Gamestate_GetFlag(GFLAG_WORLD_POS_X),
		Gamestate_GetFlag(GFLAG_WORLD_POS_Y),
		Gamestate_GetFlag(GFLAG_WORLD_DIR)
	);
}


//	Scene Termination
void scn_world_teardown() {
	Sound_OST_ClearQueue();
	Sound_OST_FadeNext(250);

	Sound_SFX_Clear(SFX_DIT_UP);
	Sound_SFX_Clear(SFX_STEP_1);
	Sound_SFX_Clear(SFX_STEP_2);
}


//	Scene Event Handler
void scn_world_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;

	World_HandleEvents(evt);

	// Check for changes to global vars
	if (Gamestate_GetFlag(GFLAG_END) > 0) {
		g_isRunning = false;
		return;
	}

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