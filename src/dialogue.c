#include "dialogue.h"

Dialogue_Tree g_CurrentDialogue = {
	.root = NULL,
	.current = NULL,
	.state = 0,
};


static Datablock_File *__curr_dia_file = NULL;
static int __curr_text_speed = 50; // Delay between typed characters in ms


void Dialogue_LoadTree(char *filename) {
	if (__curr_dia_file != NULL) {
		char msg[256];
		SDL_snprintf(msg, 256,
			"Loading new Dialogue-Tree (%s) before last one (%s) was unloaded; Last one will be unloaded!",
			filename, __curr_dia_file->filename
		);
		Log_Message(LOG_WARNING, msg);
		Dialogue_UnloadTree();
	}

	__curr_dia_file = Datablock_File_Open(filename);
	if (__curr_dia_file == NULL) return;

	// Load header block
	Datablock *db_header = Datablock_File_GetBlock(__curr_dia_file, 0);
	char *err = NULL;
	if (db_header == NULL) err = "File has no blocks";
	if (db_header->block_type != DIA_FILE_HEADER_DBID) err = "Header has wrong type";
	if (db_header->block_length < DIA_FILE_HEADER_SIZE) err = "Header is too small";
	if (err != NULL) {
		char msg[256];
		SDL_snprintf(msg, 256,
			"Failed loading invalid Dialogue File (%s): %s",
			filename, err
		);
		Log_Message(LOG_ERROR, msg);
		Dialogue_UnloadTree();
		return;
	}
	if (!Datablock_IsValid(db_header)) {

		// DEBUG: Makes it easier for manual editing to know the right checksum, lol
		Datablock_CalcSum(db_header);

		char msg[256];
		SDL_snprintf(msg, 256,
			"Dialogue File Header (%s) has an invalid checksum (Should be 0x%08X)",
			filename, db_header->checksum
		);
		Log_Message(LOG_WARNING, msg);
	}


	// Parse header
	void *data = db_header->data;

	NodeID root_node_id;
	SDL_memcpy(&root_node_id, data, sizeof(root_node_id));
	data += sizeof(root_node_id);

	Datablock_Destroy(db_header);

	// Load Root
	g_CurrentDialogue.root = Dialogue_LoadNode(__curr_dia_file, root_node_id);
	if (g_CurrentDialogue.root == NULL) {
		Dialogue_UnloadTree();
		return;
	}
}

void Dialogue_UnloadTree() {
	Dialogue_FreeNode(g_CurrentDialogue.root);
	g_CurrentDialogue.root = NULL;
	Dialogue_FreeNode(g_CurrentDialogue.current);
	g_CurrentDialogue.current = NULL;
	g_CurrentDialogue.state = 0;

	if (__curr_dia_file != NULL) {
		Datablock_File_Close(__curr_dia_file);
		__curr_dia_file = NULL;
	}
}

void Dialogue_HandleEvents(SDL_Event event) {
	if (g_CurrentDialogue.root == NULL) return;

	static bool key_held = false;
	//static bool done_next = false;
	switch (event.type) {
		case SDL_KEYDOWN: {
			if (key_held) break;

			Input_Type in = Binding_ConvKeyCode(event.key.keysym.sym);
			key_held = true;
			if (in == INPUT_SELECT) {
			
				// Start Dialogue
				//if (g_CurrentDialogue.current == NULL) {
				//	g_CurrentDialogue.current = g_CurrentDialogue.root;
				//	g_CurrentDialogue.current->state = 0;
				//	return;
				//}

				//// Skip Typing
				//int state = g_CurrentDialogue.current->state;
				//if (state > 0) {
				//	g_CurrentDialogue.current->state = -1;
				//	return;
				//}

				//// DEBUG: Skip to next node
				//if (!done_next && state < 0) {
				//	g_CurrentDialogue.current = Dialogue_LoadNode(__curr_dia_file, 0x0002);
				//	done_next = true;
				//	return;
				//}

				//// Done, go back to start
				//Dialogue_FreeNode(g_CurrentDialogue.current);
				//g_CurrentDialogue.current = NULL;
				//done_next = false;
			}
		} break;

		case SDL_KEYUP: {
			//Input_Type in = Binding_ConvKeyCode(event.key.keysym.sym);
			key_held = false;
		} break;
	}
}

void Dialogue_DrawAll() {
	if (g_CurrentDialogue.root == NULL) return;
	if (g_CurrentDialogue.current == NULL) return;

	Dialogue_DrawNode(g_CurrentDialogue.current);
}


//	Internal function to look up datablock indices in the userdata file
//	
//	returns -1 if the block was not found, or the file is not yet loaded
//	Duplicate from UserData (ADD THIS TO GT!!!)
static int __Node_Lookup(Datablock_File *dbf, Uint16 type) {
	if (dbf == NULL) return -1;
	if (type == NULL_NODE) return -1;

	for (int i=0; i<dbf->num_blocks; i++) {
		Datablock_IndexEntry entry = dbf->index[i];
		if (entry.block_type == type) return i;
	}

	return -1;
}


// TODO: Better Error Handling
Dialogue_Node *Dialogue_LoadNode(Datablock_File *dbf, NodeID dbid) {
	int block_index = __Node_Lookup(dbf, dbid);
	if (block_index < 0) {
		char msg[256];
		SDL_snprintf(msg, 256,
			"Failed to load Dialogue Node (0x%04X): No block with that ID in '%s'",
			dbid, dbf->filename
		);
		Log_Message(LOG_ERROR, msg);
		return NULL;
	}

	Datablock *db_node = Datablock_File_GetBlock(__curr_dia_file, block_index);
	if (db_node == NULL) {
		char msg[256];
		SDL_snprintf(msg, 256,
			"Failed to load Dialogue Node (0x%04X): Couldn't get block with index %i",
			dbid, block_index
		);
		Log_Message(LOG_ERROR, msg);
		return NULL;
	}

	if (!Datablock_IsValid(db_node)) {

		// DEBUG: Makes it easier for manual editing to know the right checksum, lol
		Datablock_CalcSum(db_node);

		char msg[256];
		SDL_snprintf(msg, 256,
			"Dialogue Node (0x%04X) has an invalid checksum (Should be 0x%08X)",
			dbid, db_node->checksum
		);
		Log_Message(LOG_WARNING, msg);
	}

	// Parse node
	Dialogue_Node *node = SDL_malloc(sizeof(Dialogue_Node));
	node->state = 0;
	
	// Copy Text
	node->text = SDL_malloc(sizeof(char) * db_node->block_length);
	SDL_memcpy(node->text, db_node->data, db_node->block_length);

	Datablock_Destroy(db_node);

	return node;
}

void Dialogue_FreeNode(Dialogue_Node *node) {
	if (node == NULL) return;
	SDL_free(node);
}

void Dialogue_DrawNode(Dialogue_Node *node) {
	if (node == NULL) {
		Log_Message(LOG_WARNING, "Tried to draw a NULL node");
		return;
	}

	Uint64 now = SDL_GetTicks64();
	static Uint64 last_tick = 0;
	if (node->state == 0) {
		node->state = 1;
		last_tick = now;
		Sound_SFX_Prepare(SFX_DIALOGUE_BEEP);
		return;
	}

	if (node->state > 0) {
		int diff = now - last_tick;
		if (diff >= __curr_text_speed) {
			node->state++;
			last_tick = now;

			// This check halves the beep speed for very fast text (cleaner sound)
			if (__curr_text_speed >= 100 || (node->state & 1)) {
				Sound_SFX_Play(SFX_DIALOGUE_BEEP, -1);
			}
		}

		if (node->state >= DIA_BOX_SIZE) {
			node->state = -1;
			Sound_SFX_Clear(SFX_DIALOGUE_BEEP);
		}
	}

	int written = TTFText_Draw_Box((TTFText_Box){
		DIA_BOX_POS_X, DIA_BOX_POS_Y,
		DIA_BOX_COLS, DIA_BOX_ROWS,
		CLR_TEXT_NORM, node->state,
		node->text
	});

	if (written < 0) {
		node->state = -1;
		Sound_SFX_Clear(SFX_DIALOGUE_BEEP);
	}

	// TODO: Draw Responses
}
