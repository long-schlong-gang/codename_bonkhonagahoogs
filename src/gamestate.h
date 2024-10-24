#ifndef GAME_STATE_H
#define GAME_STATE_H

//	
//	Holds all the important inter-scene game state data
//	Also handles writing this data to the user's saves
//	

#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>

#include "userdata.h"


////	Constants

//		General Flag Assignment Guideline
//		
//	0x0?	Important Meta/Engine Flags
//	0x1?	Current Dialogue Flags
//	0x2?	Current World/Tile Flags
// >0x8?	Story-relevant flags

#define GFLAG_NULL			(Uint8)(0x00)
#define GFLAG_ACT_NUM		(Uint8)(0x01)
#define GFLAG_BAD_END		(Uint8)(0xFF)	// Meta: >0 if you mega fucked up.

#define GFLAG_S_ERUYA_TRUST	(Uint8)(0x81)	// Story: >0 if Eruya trusts you as the captain; (Set by checking her cargo)


////	Types

typedef enum {
	GAME_SLOT_NONE,
	GAME_SLOT_1,
	GAME_SLOT_2,
	GAME_SLOT_3,
} Game_Save;

typedef struct {
	char *scripted_next_scene;
	Uint8 global_flags[0x100];
} Game_State;


////	Global Vars

extern Game_State g_CurrentGame;


////	Public Functions

//	Initialises Game State
//	
//	Loads game state info from the userdata file at
//	the slot specified by `save`
void Gamestate_Load(Game_Save save);

//	Saves Game State to Userdata
//	
//	Overwrites the data in the given slot
void Gamestate_Save(Game_Save save);

//	Terminates Game State
//	
//	NEEDED??
//void Gamestate_Term();

//	Resets the current game state to that of a new game
//	
void Gamestate_NewGame();

//	Sets a global flag's value
//	
//	Makes no changes, if `key` is GFLAG_NULL
void Gamestate_SetFlag(Uint8 key, Uint8 val);

//	Gets a global flag's value
//	
//	Always returns 0x00, if `key` is GFLAG_NULL
Uint8 Gamestate_GetFlag(Uint8 key);


#endif