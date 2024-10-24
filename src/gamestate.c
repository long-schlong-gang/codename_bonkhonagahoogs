#include "gamestate.h"


Game_State g_CurrentGame = {
	.scripted_next_scene = NULL,
};


void Gamestate_NewGame() {
}

void Gamestate_SetFlag(Uint8 key, Uint8 val) {
	if (key == GFLAG_NULL) return;
	g_CurrentGame.global_flags[key] = val;

	char msg[256];
	SDL_snprintf(msg, 256, "Global flag 0x%02X was set to 0x%02X", key, val);
	Log_Message(LOG_DEBUG, msg);
}

Uint8 Gamestate_GetFlag(Uint8 key) {
	if (key == GFLAG_NULL) return 0x00;
	return g_CurrentGame.global_flags[key];
}