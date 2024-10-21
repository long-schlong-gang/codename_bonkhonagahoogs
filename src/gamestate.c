#include "gamestate.h"


Game_State g_CurrentGame = {
	.scripted_next_scene = NULL,
};


void Gamestate_NewGame() {
	Dialogue_LoadTree(DIAF_INTRO_CUTSCENE);
	g_CurrentDialogue.background = PIX_TITLE_SPLASH;
	Pix_Load(PIX_TITLE_SPLASH);

	g_CurrentGame.scripted_next_scene = "world";
	Scene_Set("dia");
}

void Gamestate_SetFlag(Uint8 key, Uint8 val) {
	if (key == GFLAG_NULL) return;
	g_CurrentGame.global_flags[key] = val;
}

Uint8 Gamestate_GetFlag(Uint8 key) {
	if (key == GFLAG_NULL) return 0x00;
	return g_CurrentGame.global_flags[key];
}