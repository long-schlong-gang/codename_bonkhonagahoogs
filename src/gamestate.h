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
//	0x3?	Character flags
// >0x8?	Story-relevant flags

#define GFLAG_NULL			(Uint8)(0x00)
#define GFLAG_ACT_NUM		(Uint8)(0x01)	// Meta: index of overall story progress
#define GFLAG_SCENE_NUM		(Uint8)(0x02)	// Meta: index of next scene to load; 0 = title, 1 = world, 2 = dialogue
#define GFLAG_CUTSCENE		(Uint8)(0x03)	// Meta: Scripted cutscene dialogue ID; 0 means none

#define GFLAG_WORLD_ROOM	(Uint8)(0x20)	// World: what room the player's in
#define GFLAG_WORLD_POS_X	(Uint8)(0x21)	// World: Player X coordinate
#define GFLAG_WORLD_POS_Y	(Uint8)(0x22)	// World: Player Y coordinate
#define GFLAG_WORLD_DIR		(Uint8)(0x23)	// World: Direction player is facing

#define GFLAG_LEVU_DIA		(Uint8)(0x30)	// NPCs: Index of Levu's current dialogue
#define GFLAG_LEVU_KARMA	(Uint8)(0x31)	// NPCs: Levu's current mood/progress (0 = Neutral, >0 = Good; <0 (>127) = Bad)
#define GFLAG_ERUYA_DIA		(Uint8)(0x34)	// NPCs: Index of Eruya's current dialogue
#define GFLAG_ERUYA_KARMA	(Uint8)(0x35)	// NPCs: Eruya's current mood/progress (0 = Neutral, >0 = Good; <0 (>127) = Bad)
#define GFLAG_FEDELOV_DIA	(Uint8)(0x38)	// NPCs: Index of Fedelov's current dialogue
#define GFLAG_FEDELOV_KARMA	(Uint8)(0x39)	// NPCs: Fedelov's current mood/progress (0 = Neutral, >0 = Good; <0 (>127) = Bad)
#define GFLAG_KELEN_DIA		(Uint8)(0x3C)	// NPCs: Index of Kelen's current dialogue
#define GFLAG_KELEN_KARMA	(Uint8)(0x3D)	// NPCs: Kelen's current mood/progress (0 = Neutral, >0 = Good; <0 (>127) = Bad)

#define GFLAG_S_CARGO_SECURE	(Uint8)(0x81)	// Story: >0 if you secured the cargo (Eruya trusts you more)
#define GFLAG_S_E_HELPED_L		(Uint8)(0x82)	// Story: >0 if you told Eruya to help Levu during the initial turbulance

#define GFLAG_END			(Uint8)(0xFF)	// Meta/Story: If >0 will end the game


#define ACT_NUM_INTRO		0
#define ACT_NUM_TURBULENCE	1
#define ACT_NUM_BRIEFING	2

#define SCENE_NUM_TITLE		0
#define SCENE_NUM_WORLD		1
#define SCENE_NUM_DIALOGUE	2

#define CUTSCENE_NUM_NONE	0
#define CUTSCENE_NUM_INTRO	1
#define CUTSCENE_NUM_OUTTRO	2


////	Types

typedef enum {
	GAME_SLOT_NONE = -1,
	GAME_SLOT_1 = 0,
	GAME_SLOT_2 = 1,
	GAME_SLOT_3 = 2,
} Game_Slot;

typedef struct {
	char name[13]; // Saves can have a 12 char name
	Uint32 playtime; // in seconds
} Game_SlotInfo;

typedef struct {
	Uint8 global_flags[0x100];
	Game_Slot curr_slot;
	Game_SlotInfo curr_slot_info;
	Uint64 start_tick; // The program tick at the start of the current loaded game
} Game_State;


////	Global Vars

extern Game_State g_CurrentGame;


////	Public Functions

//	Initialises Game State
//	
//	Loads game state info from the userdata file at
//	the slot specified by `save`
//	
//	Does nothing if `save` is SLOT_NONE
void Gamestate_Load(Game_Slot save);

//	Saves Game State to Userdata
//	
//	Overwrites the data in the given slot
void Gamestate_Save(Game_Slot save);

//	Load the next "scene" based on the current flags
//	
void Gamestate_NextScene();

//	Terminates Game State
//	
//	NEEDED??
//void Gamestate_Term();

//	Resets the current game state to that of a new game
//	
void Gamestate_NewGame();

//	Gets info about a certain save slot
//	
Game_SlotInfo Gamestate_SlotInfo(Game_Slot save);

//	Checks if a slot is "empty"
//	
//	A slot is considered empty when the name and time fields are all 0s
//	SLOT_NONE is always "empty"
bool Gamestate_SlotEmpty(Game_Slot save);

//	Sets a global flag's value
//	
//	Makes no changes, if `key` is GFLAG_NULL
void Gamestate_SetFlag(Uint8 key, Uint8 val);

//	Gets a global flag's value
//	
//	Always returns 0x00, if `key` is GFLAG_NULL
Uint8 Gamestate_GetFlag(Uint8 key);


#endif