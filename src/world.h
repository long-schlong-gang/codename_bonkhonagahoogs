#ifndef WORLD_H
#define WORLD_H

//	
//	Creates a top-down 2D world interface for moving around
//	the ship and interacting with NPCs
//	

#include <SDL2/SDL.h>
#include <screen.h>
#include <binding.h>

#include "pix.h"
#include "colours.h"
#include "ttf_text.h"
#include "dialogue.h"
#include "gamestate.h"


////	Constants
#define TILE_COUNT 0x14
#define ROOM_COUNT 0x09

#define WORLD_TILE_SIZE 100
#define WORLD_PADDING_X 50
#define WORLD_PADDING_Y 50

////	Types

typedef enum {
	TILE_VOID,
	TILE_WALKWAY,
} World_TileID;

typedef enum {
	ROOM_QUART_CAPTAIN,
	ROOM_BRIDGE,
	ROOM_HALLWAY,
	ROOM_QUART_LEVU,
	ROOM_QUART_KELEN,
	ROOM_QUART_FEDELOV,
	ROOM_QUART_ERUYA,
	ROOM_CAFETERIA,
	ROOM_CARGO_HOLD,
} World_RoomID;

typedef void (*World_Callback)(void *udata);

typedef struct {
	World_Callback on_enter;
	World_Callback on_exit;
	World_Callback on_interact;
	void *udata;
	char *facing_tooltip;
	bool walkable;
	int tile_img; // None if negative
} World_Tile;

typedef struct {
	Image background;
	int w;
	int h;
	Uint8 *tile_ids;
} World_Room;

typedef enum {
	DIR_UP,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_DOWN,
} World_Direction;

typedef struct {
	World_Direction dir;
	int x;
	int y;
} World_Player;

////	Callback struct params

struct world_dest_s {
	World_RoomID rm;
	int x; int y;
	int dir;
};

struct world_text_s {
	char *str;
	int x; int y;
	PaletteColour clr;
};

typedef struct {
	World_RoomID curr_room;
	World_Player player;
	int room_x; // Precalc'd offset to centre room
	int room_y; // Precalc'd offset to centre room
	char *dialogue_filename;
	char *txt;
} World_Zawarudo;

////	Global Vars

extern World_Tile g_WorldTiles[TILE_COUNT];
extern World_Room g_WorldRooms[ROOM_COUNT];
extern World_Zawarudo g_World;
extern bool g_World_Debug;


////	Public Functions

//	Initialise World System
//	
//void World_Init();

//	Terminate World System
//	
//void World_Term();

//	Sets the player's position in the world
//	
//	If x or y are out of the room, the player defaults to the
//	first (top-left most) walkable tile.
//	Set `dir` to -1, to leave it unchanged
//	
//	Does nothing with invalid params
void World_Teleport(World_RoomID rm_id, int x, int y, int dir);

//	Draws a small single-line textbox over the given tile
//	
//	Good for basic world interaction
void World_DrawText(char *str, int tile_x, int tile_y, PaletteColour clr);

//	Handles the inputs for world interaction
//	
void World_HandleEvents(SDL_Event event);

//	Draws the current world info to the screen
//	
void World_Draw();

//	Draws a room to the screen
//	
void World_DrawRoom(World_RoomID room);

//	Returns the ID of the tile directly in front of the player
//	
//	If `x` and/or `y` is not NULL, they will be set with
//	the coordinates of the tile.
World_TileID World_GetFacingTile(int *x, int *y);


////	Callback Functions

//	Callback to move between rooms
//	
//	`_dest` is a pointer to a world_dest_s struct
void World_CB_Teleport(void *_dest);

//	Callback to display a simple text box
//	
//	`_text` is char pointer
void World_CB_Textbox(void *_text);

//	Callback to start a dialogue
//	
//	`_filename` is a char pointer with the dialogue's filename
void World_CB_Dialogue(void *_filename);

#endif