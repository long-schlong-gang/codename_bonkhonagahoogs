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


////	Constants
#define TILE_COUNT 3
#define ROOM_COUNT 1

#define WORLD_TILE_SIZE 100
#define WORLD_PADDING_X 50
#define WORLD_PADDING_Y 50

////	Types

typedef enum {
	TILE_VOID,
	TILE_WALKWAY,
} World_TileID;

typedef enum {
	ROOM_TEST,
} World_RoomID;


typedef void (*World_Callback)(void *udata);

typedef struct {
	World_Callback on_enter;
	World_Callback on_exit;
	World_Callback on_interact;
	void *udata;
	bool walkable;
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

typedef struct {
	World_RoomID curr_room;
	World_Player player;
	int room_x; // Precalc'd offset to centre room
	int room_y; // Precalc'd offset to centre room
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

//	Handles the inputs for world interaction
//	
void World_HandleEvents(SDL_Event event);

//	Draws the current world info to the screen
//	
void World_Draw();

//	Draws a room to the screen
//	
void World_DrawRoom(World_RoomID room);


////	Callback Functions

#endif