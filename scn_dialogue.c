
#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>

#include "src/menu_element.h"
#include "src/ttf_text.h"
#include "src/dialogue.h"
#include "src/sound.h"
#include "src/pix.h"


//	Scene Initialisation
void scn_dialogue_setup() {
	Dialogue_LoadTree(DIALOGUE_FILENAME);
	Dialogue_Start();

	Pix_Load(PIX_ERUYA_SKETCH);
}


//	Scene Termination
void scn_dialogue_teardown() {
	Pix_Clear(PIX_ERUYA_SKETCH);

	Dialogue_UnloadTree();
}


//	Scene Event Handler
void scn_dialogue_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;

	Dialogue_HandleEvents(evt);

	if (g_CurrentDialogue.current == NULL) {
		Scene_Set("title");
	}
}


//	Scene Draw Calls
void scn_dialogue_draw_frame() {
	Colours_SetRenderer(CLR_WINDOW_BG);
	SDL_RenderClear(g_renderer);

	Pix_Draw(PIX_ERUYA_SKETCH, 50, 50, -1, -1);

	Dialogue_DrawAll();
}


//	Global Scene Handle
Scene scn_dialogue = {
	.setup = &scn_dialogue_setup,
	.teardown = &scn_dialogue_teardown,
	.handle_events = &scn_dialogue_handle_events,
	.draw_frame = &scn_dialogue_draw_frame,
};