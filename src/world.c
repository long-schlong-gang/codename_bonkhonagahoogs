#include "world.h"


void __cb_test_callback(void *_) {
	puts("Hello, world!");
	fflush(stdout);
}


World_Tile g_WorldTiles[TILE_COUNT] = {
	{	// Ground Tile
		.on_enter = NULL,
		.on_exit = NULL,
		.on_interact = NULL,
		.udata = NULL,
		.walkable = true,
	},
	{	// Test Tile
		.on_enter = &__cb_test_callback,
		.on_exit = NULL,
		.on_interact = NULL,
		.udata = NULL,
		.walkable = true,
	},
	{	// Test Tile 2
		.on_enter = NULL,
		.on_exit = NULL,
		.on_interact = &__cb_test_callback,
		.udata = NULL,
		.walkable = false,
	},
};

World_Room g_WorldRooms[ROOM_COUNT] = {
	{
		.background = PIX_TIT_SMILE,
		.w = 5,
		.h = 5,
		.tile_ids = (Uint8 [25]){
			0x01, 0x01, 0x01, 0x01, 0x01,
			0x01, 0x02, 0x01, 0x01, 0x03,
			0x01, 0x01, 0x01, 0x01, 0x01,
			0x01, 0x01, 0x00, 0x01, 0x01,
			0x01, 0x01, 0x01, 0x01, 0x01,
		}
	},
};

World_Zawarudo g_World = {
	.curr_room = ROOM_TEST,
	.player = {
		.dir = DIR_DOWN,
		.x = 3,
		.y = 3,
	},
	.room_x = 0,
	.room_y = 0,
};

bool g_World_Debug = false;


//void Colours_Init();

//void Colours_Term();

static void __World_InteractTile() {
	int new_x = g_World.player.x;
	int new_y = g_World.player.y;

	switch (g_World.player.dir) {
		case DIR_UP: new_y--; break;
		case DIR_LEFT: new_x--; break;
		case DIR_RIGHT: new_x++; break;
		case DIR_DOWN: new_y++; break;
	}

	World_Room rm = g_WorldRooms[g_World.curr_room];
	int tile_index = new_y * rm.w + new_x;
	World_TileID tile_id = rm.tile_ids[tile_index];
	if (tile_id == 0) return;

	World_Tile tl = g_WorldTiles[tile_id - 1];
	if (tl.on_interact != NULL) tl.on_interact(tl.udata);
}

void World_HandleEvents(SDL_Event event) {

	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_F5) g_World_Debug = !g_World_Debug;

		int new_x = g_World.player.x;
		int new_y = g_World.player.y;

		Input_Type in = Binding_ConvKeyCode(event.key.keysym.sym);
		switch (in) {
			case INPUT_UP: new_y--; g_World.player.dir = DIR_UP; break;
			case INPUT_LEFT: new_x--; g_World.player.dir = DIR_LEFT; break;
			case INPUT_RIGHT: new_x++; g_World.player.dir = DIR_RIGHT; break;
			case INPUT_DOWN: new_y++; g_World.player.dir = DIR_DOWN; break;
			case INPUT_SELECT: __World_InteractTile(); return;
			default: return;
		}

		World_Room rm = g_WorldRooms[g_World.curr_room];
		if (new_x < 0 || new_x >= rm.w) return;
		if (new_y < 0 || new_y >= rm.h) return;

		// Check if tile is void or not
		int tile_index = new_y * rm.w + new_x;
		World_TileID tile_id = rm.tile_ids[tile_index];
		if (tile_id == 0) return;
		World_Tile tl = g_WorldTiles[tile_id - 1];
		if (!tl.walkable) return;

		// Move the player
		g_World.player.x = new_x;
		g_World.player.y = new_y;

		// Trigger Callbacks
		if (tl.on_enter != NULL) tl.on_enter(tl.udata);

		tile_index = g_World.player.y * rm.w + g_World.player.x;
		tile_id = rm.tile_ids[tile_index];
		if (tile_id != 0) {
			World_Tile prev_tile = g_WorldTiles[tile_id - 1];
			if (prev_tile.on_exit != NULL) prev_tile.on_exit(prev_tile.udata);
		}
	}

}

void World_Draw() {
	World_DrawRoom(g_World.curr_room);

	Image img = PIX_TIT_SMILE;
	switch (g_World.player.dir) {
		case DIR_UP: img = PIX_PLAYER_UP; break;
		case DIR_LEFT: img = PIX_PLAYER_LEFT; break;
		case DIR_RIGHT: img = PIX_PLAYER_RIGHT; break;
		case DIR_DOWN: img = PIX_PLAYER_DOWN; break;
	}

	Pix_Draw(img,
		g_World.player.x * WORLD_TILE_SIZE + g_World.room_x,
		g_World.player.y * WORLD_TILE_SIZE + g_World.room_y,
		WORLD_TILE_SIZE, WORLD_TILE_SIZE
	);
}


void World_DrawRoom(World_RoomID room) {
	World_Room rm = g_WorldRooms[room];

	if (g_World.room_x == 0) {
		g_World.room_x = (g_screen_width - 2*WORLD_PADDING_X)/2 - (rm.w * WORLD_TILE_SIZE)/2 + WORLD_PADDING_X;
	}
	if (g_World.room_y == 0) {
		g_World.room_y = (g_screen_height - 2*WORLD_PADDING_Y)/2 - (rm.h * WORLD_TILE_SIZE)/2 + WORLD_PADDING_Y;
	}

	Pix_Draw(rm.background,
		g_World.room_x,
		g_World.room_y,
		rm.w * WORLD_TILE_SIZE,
		rm.h * WORLD_TILE_SIZE
	);

	if (g_World_Debug) {
		for (int y=0; y<rm.h; y++) {
			for (int x=0; x<rm.w; x++) {
				SDL_SetRenderDrawColor(g_renderer, 0xFF, 0x80, 0x00, 0x40);
				if (rm.tile_ids[y * rm.w + x] == TILE_VOID) {
					SDL_SetRenderDrawColor(g_renderer, 0xFF, 0x00, 0x00, 0x40);
				}
				if (rm.tile_ids[y * rm.w + x] == TILE_WALKWAY) {
					SDL_SetRenderDrawColor(g_renderer, 0x00, 0xFF, 0x00, 0x40);
				}

				// Outline
				SDL_RenderDrawRect(g_renderer, &(struct SDL_Rect){
					x * WORLD_TILE_SIZE + g_World.room_x,
					y * WORLD_TILE_SIZE + g_World.room_y,
					WORLD_TILE_SIZE, WORLD_TILE_SIZE
				});
				SDL_RenderDrawRect(g_renderer, &(struct SDL_Rect){
					x * WORLD_TILE_SIZE + g_World.room_x + 1,
					y * WORLD_TILE_SIZE + g_World.room_y + 1,
					WORLD_TILE_SIZE-2, WORLD_TILE_SIZE-2
				});

				// Fill
				SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);
				SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
					x * WORLD_TILE_SIZE + g_World.room_x,
					y * WORLD_TILE_SIZE + g_World.room_y,
					WORLD_TILE_SIZE, WORLD_TILE_SIZE
				});
				SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_NONE);
			}
		}
	}
}