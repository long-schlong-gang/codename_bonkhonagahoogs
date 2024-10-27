#include "gamestate.h"


Game_State g_CurrentGame = {
	.curr_slot = GAME_SLOT_NONE,
	.curr_slot_info = {
		.name = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
		.playtime = 0,
	},
	.start_tick = 0,
};


void Gamestate_Load(Game_Slot save) {
	if (save == GAME_SLOT_NONE) return;

	if (Gamestate_SlotEmpty(save)) {
		// TODO: Set up new game
	}

	g_CurrentGame.curr_slot = save;
	g_CurrentGame.curr_slot_info = Gamestate_SlotInfo(save);
	UserData_Get(UDATA_DBID_SAVEDATA, save, g_CurrentGame.global_flags, 256);
	g_CurrentGame.start_tick = SDL_GetTicks64();
}

void Gamestate_Save(Game_Slot save) {
	if (save == GAME_SLOT_NONE) return;

	// Update Slot info
	Uint8 buffer[48];
	UserData_Get(UDATA_DBID_SAVEINFO, 0, buffer, 48);
	Uint8 *data = &buffer[16 * save];

	char *name = "Fermata\0\0\0\0";
	switch (Gamestate_GetFlag(GFLAG_ACT_NUM)) {
		case ACT_NUM_TURBULENCE:	name = "Turbulence\0"; break;
		case ACT_NUM_BRIEFING:		name = "Briefing\0\0\0"; break;
	}
	SDL_memcpy(data, name, 12);

	Uint32 elapsed_sec = (SDL_GetTicks64() - g_CurrentGame.start_tick) / 1000;
	*(Uint32 *)(data + 12) = g_CurrentGame.curr_slot_info.playtime + elapsed_sec;

	UserData_Set(UDATA_DBID_SAVEINFO, 0, buffer, 48);

	// Update flags
	UserData_Set(UDATA_DBID_SAVEDATA, save, g_CurrentGame.global_flags, 256);
}

//void Gamestate_StartAct() {
//	switch (Gamestate_GetFlag(GFLAG_ACT_NUM)) {
//
//		case ACT_NUM_INTRO: {
//
//			Gamestate_SetFlag(GFLAG_ACT_NUM, ACT_NUM_TURBULENCE);
//			return;
//		} break;
//
//		case ACT_NUM_TURBULENCE: {
//
//		} break;
//	}
//}

void Gamestate_NextScene() {
	switch (Gamestate_GetFlag(GFLAG_SCENE_NUM)) {
		case SCENE_NUM_TITLE: Scene_Set("title"); break;
		case SCENE_NUM_WORLD: Scene_Set("world"); break;
		case SCENE_NUM_DIALOGUE: Scene_Set("dia"); break;
	}
}

Game_SlotInfo Gamestate_SlotInfo(Game_Slot save) {
	Game_SlotInfo info = {
		.name = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, },
		.playtime = 0,
	};

	if (save == GAME_SLOT_NONE) return info;

	Uint8 buffer[48];
	UserData_Get(UDATA_DBID_SAVEINFO, 0, buffer, 48);
	Uint8 *data = &buffer[16 * save];

	info.playtime = *(Uint32 *)(data + 12),
	memcpy(info.name, data, 12);
	info.name[12] = '\0';

	return info;
}

bool Gamestate_SlotEmpty(Game_Slot save) {
	if (save == GAME_SLOT_NONE) return true;

	Uint8 buffer[48];
	UserData_Get(UDATA_DBID_SAVEINFO, 0, buffer, 48);
	Uint8 *data = &buffer[16 * save];

	for (int i=0; i<16; i++) {
		if (data[i] != 0x00) return false;
	}

	return true;
}

void Gamestate_SetFlag(Uint8 key, Uint8 val) {
	if (key == GFLAG_NULL) return;
	g_CurrentGame.global_flags[key] = val;

	//char msg[256];
	//SDL_snprintf(msg, 256, "Global flag 0x%02X was set to 0x%02X", key, val);
	//Log_Message(LOG_DEBUG, msg);
}

Uint8 Gamestate_GetFlag(Uint8 key) {
	if (key == GFLAG_NULL) return 0x00;
	return g_CurrentGame.global_flags[key];
}