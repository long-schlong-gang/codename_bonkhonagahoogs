#include "world.h"

#include "world_data"

World_Zawarudo g_World = {
	.curr_room = ROOM_QUART_CAPTAIN,
	.player = {
		.dir = DIR_DOWN,
		.x = 3,
		.y = 3,
	},
	.room_x = 0,
	.room_y = 0,
};

bool g_World_Debug = false;


void World_Teleport(World_RoomID rm_id, int x, int y, int dir) {
	if (rm_id < 0 || rm_id >= ROOM_COUNT) return;

	World_Room rm = g_WorldRooms[g_World.curr_room];
	Pix_Clear(rm.background);

	rm = g_WorldRooms[rm_id];
	Pix_Load(rm.background);
	g_World.curr_room = rm_id;

	g_World.player.x = x;
	g_World.player.y = y;

	// reset cached draw position
	g_World.room_x = 0;
	g_World.room_y = 0;

	if (dir >= 0) g_World.player.dir = dir;
}

void World_DrawText(char *str, int tile_x, int tile_y, PaletteColour clr) {
	int len = SDL_utf8strlen(str);
	TTFText_Draw_Box((TTFText_Box){
		.x = tile_x * WORLD_TILE_SIZE + g_World.room_x + WORLD_TILE_SIZE/2 - (len * TTFText_GlyphWidth())/2,
		.y = tile_y * WORLD_TILE_SIZE + g_World.room_y + WORLD_TILE_SIZE - 5,
		.rows = 1,
		.cols = len,
		.clr = clr,
		.str = str,
		.charcount = -1,
	});
}

World_TileID World_GetFacingTile(int *x, int *y) {
	int new_x = g_World.player.x;
	int new_y = g_World.player.y;

	switch (g_World.player.dir) {
		case DIR_UP: new_y--; break;
		case DIR_LEFT: new_x--; break;
		case DIR_RIGHT: new_x++; break;
		case DIR_DOWN: new_y++; break;
	}

	if (x != NULL) *x = new_x;
	if (y != NULL) *y = new_y;

	World_Room rm = g_WorldRooms[g_World.curr_room];
	int tile_index = new_y * rm.w + new_x;
	return rm.tile_ids[tile_index];
}

static void __World_InteractTile() {
	World_TileID tile_id = World_GetFacingTile(NULL, NULL);
	if (tile_id == 0) return;

	World_Tile tl = g_WorldTiles[tile_id - 1];
	if (tl.on_interact != NULL) tl.on_interact(tl.udata);
}

void World_HandleEvents(SDL_Event event) {

	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_F5) g_World_Debug = !g_World_Debug;
		//if (event.key.keysym.sym == SDLK_F4) World_Teleport(ROOM_BRIDGE, 3, 0, DIR_DOWN);

		int new_x = g_World.player.x;
		int new_y = g_World.player.y;

		Input_Type in = Binding_ConvKeyCode(event.key.keysym.sym);
		switch (in) {
			case INPUT_UP: new_y--; g_World.player.dir = DIR_UP; break;
			case INPUT_LEFT: new_x--; g_World.player.dir = DIR_LEFT; break;
			case INPUT_RIGHT: new_x++; g_World.player.dir = DIR_RIGHT; break;
			case INPUT_DOWN: new_y++; g_World.player.dir = DIR_DOWN; break;
			case INPUT_SELECT: {
				if (g_World.txt != NULL) { g_World.txt = NULL; return; }
				__World_InteractTile();
			} return;
			default: return;
		}

		if (g_World.txt != NULL) {
			g_World.txt = NULL;
			return;
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

	int face_x, face_y;
	World_TileID tile_id = World_GetFacingTile(&face_x, &face_y);
	if (tile_id != TILE_VOID) {
		World_Tile tl = g_WorldTiles[tile_id - 1];

		// Highlight interactible tiles
		if (tl.on_interact != NULL) {
			Colours_SetRenderer(CLR_TILE_ITABLE);
			SDL_RenderDrawRect(g_renderer, &(struct SDL_Rect){
				face_x * WORLD_TILE_SIZE + g_World.room_x,
				face_y * WORLD_TILE_SIZE + g_World.room_y,
				WORLD_TILE_SIZE, WORLD_TILE_SIZE
			});
			SDL_RenderDrawRect(g_renderer, &(struct SDL_Rect){
				face_x * WORLD_TILE_SIZE + g_World.room_x + 1,
				face_y * WORLD_TILE_SIZE + g_World.room_y + 1,
				WORLD_TILE_SIZE-2, WORLD_TILE_SIZE-2
			});

			SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_BLEND);
			SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
				face_x * WORLD_TILE_SIZE + g_World.room_x,
				face_y * WORLD_TILE_SIZE + g_World.room_y,
				WORLD_TILE_SIZE, WORLD_TILE_SIZE
			});
			SDL_SetRenderDrawBlendMode(g_renderer, SDL_BLENDMODE_NONE);
		}

		if (tl.facing_tooltip != NULL) {
			World_DrawText(tl.facing_tooltip, face_x, face_y, CLR_TILE_ITABLE);
		}
	}

	if (g_World.txt != NULL) {
		TTFText_Draw_Box((TTFText_Box){
			DIA_BOX_PADDING,
			g_screen_height - DIA_BOX_PADDING - (DIA_BOX_ROWS * TTFText_GlyphHeight()),
			DIA_BOX_COLS, DIA_BOX_ROWS,
			CLR_TEXT_NORM, -1,
			g_World.txt
		});

		//World_DrawText(
		//	g_World.txt->str,
		//	g_World.txt->x,
		//	g_World.txt->y,
		//	g_World.txt->clr
		//);
	}

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

void World_CB_Teleport(void *_dest) {
	if (_dest == NULL) return;
	struct world_dest_s dest = *(struct world_dest_s *)(_dest);

	World_Teleport(dest.rm, dest.x, dest.y, dest.dir);
}

void World_CB_Textbox(void *_text) {
	if (_text == NULL) return;
	g_World.txt = (char *)(_text);
}