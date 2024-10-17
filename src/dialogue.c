#include "dialogue.h"

Dialogue_Tree g_CurrentDialogue = {
	//.root = NULL,
	.root_id = NULL_NODE,
	.current = NULL,
	.next_node = -1,
	.response_buttons = NULL,
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
	g_CurrentDialogue.root_id = root_node_id;

	Datablock_Destroy(db_header);

	// Load Root
	//g_CurrentDialogue.current = Dialogue_LoadNode(__curr_dia_file, root_node_id);
	//if (g_CurrentDialogue.current == NULL) {
	//	Dialogue_UnloadTree();
	//	return;
	//}
}

void Dialogue_UnloadTree() {
	//Dialogue_FreeNode(g_CurrentDialogue.root);
	//g_CurrentDialogue.root = NULL;
	Dialogue_FreeNode(g_CurrentDialogue.current);
	g_CurrentDialogue.current = NULL;
	Menel_TBtnArr_Destroy(g_CurrentDialogue.response_buttons);
	g_CurrentDialogue.response_buttons = NULL;

	if (__curr_dia_file != NULL) {
		Datablock_File_Close(__curr_dia_file);
		__curr_dia_file = NULL;
	}
}

void Dialogue_Start() {
	g_CurrentDialogue.current = Dialogue_LoadNode(__curr_dia_file, g_CurrentDialogue.root_id);
	if (g_CurrentDialogue.current == NULL) {
		Dialogue_UnloadTree();
		return;
	}
}

void Dialogue_GoToNode(NodeID node) {
	if (g_CurrentDialogue.current == NULL) return;

	if (node == 0x0000) {
		Dialogue_UnloadTree();
		return;
	}

	Menel_TBtnArr_Destroy(g_CurrentDialogue.response_buttons);
	g_CurrentDialogue.response_buttons = NULL;

	Dialogue_FreeNode(g_CurrentDialogue.current);
	g_CurrentDialogue.current = Dialogue_LoadNode(__curr_dia_file, node);
	if (g_CurrentDialogue.current == NULL) Dialogue_UnloadTree();
}

static void __CB_GoToNode(void *_nodeid) {
	g_CurrentDialogue.next_node = 0x0000;
	if (_nodeid != NULL) g_CurrentDialogue.next_node = *(NodeID *)(_nodeid);
}

void Dialogue_HandleEvents(SDL_Event event) {
	//if (g_CurrentDialogue.root == NULL) return;
	if (g_CurrentDialogue.current == NULL) return;

	Menel_TBtnArr_HandleEvent(g_CurrentDialogue.response_buttons, event);

	static bool key_held = false;
	switch (event.type) {
		case SDL_KEYDOWN: {
			if (key_held) break;

			Input_Type in = Binding_ConvKeyCode(event.key.keysym.sym);
			key_held = true;
			if (in == INPUT_SELECT) {
			
				// Skip Typing
				int state = g_CurrentDialogue.current->state;
				if (state > 0) {
					g_CurrentDialogue.current->state = -1;
					return;
				}
			}
		} break;

		case SDL_KEYUP: {
			Input_Type in = Binding_ConvKeyCode(event.key.keysym.sym);
			if (in == INPUT_SELECT) {
				key_held = false;
			}
		} break;
	}

	if (g_CurrentDialogue.next_node >= 0) {
		NodeID node = g_CurrentDialogue.next_node;
		Dialogue_GoToNode(node);
		g_CurrentDialogue.next_node = -1;
	}
}

void Dialogue_DrawAll() {
	//if (g_CurrentDialogue.root == NULL) return;
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

	// Parse node Header
	void *data = db_node->data;
	void *end = db_node->data + db_node->block_length;
	Dialogue_Node *node = SDL_malloc(sizeof(Dialogue_Node));
	node->state = 0;

	Uint8 text_len;
	SDL_memcpy(&text_len, data, sizeof(text_len));
	data += sizeof(text_len);

	data += 3; // Skip blank node header bytes

	// Copy Text
	node->text = SDL_malloc(sizeof(char) * text_len + 1);
	SDL_memcpy(node->text, data, text_len);
	node->text[text_len] = '\0';
	data += text_len;

	// Parse Responses
	Dialogue_Response res_buf[DIA_MAX_RESPONSES];
	node->num_responses = 0;
	while (data < end) {
		Dialogue_Response *res = &res_buf[node->num_responses];
		res->next = 0x0000;
		res->text = NULL;
		node->num_responses++;

		SDL_memcpy(&res->next, data, sizeof(NodeID));
		data += sizeof(NodeID);

		if (data >= end) continue;

		Uint8 res_txt_len;
		SDL_memcpy(&res_txt_len, data, sizeof(Uint8));
		data += sizeof(Uint8);

		data += 1; // Skip unused response header bytes

		res->text = SDL_malloc(sizeof(char) * res_txt_len + 1);
		SDL_memcpy(res->text, data, res_txt_len);
		res->text[res_txt_len] = '\0';
		data += res_txt_len;
	}

	if (node->num_responses > 0) {
		node->responses = SDL_malloc(sizeof(Dialogue_Node) * node->num_responses);
		for (int i=0; i<node->num_responses; i++) {
			node->responses[i].next = res_buf[i].next;
			node->responses[i].text = res_buf[i].text;
		}
	}

	Datablock_Destroy(db_node);

	return node;
}

void Dialogue_FreeNode(Dialogue_Node *node) {
	if (node == NULL) return;

	// TODO: Free responses, resp text and text?
	if (node->responses != NULL) {
		for (int i=0; i<node->num_responses; i++) {
			if (node->responses[i].text != NULL) SDL_free(node->responses[i].text);
		}
		SDL_free(node->responses);
	}

	if (node->text != NULL) SDL_free(node->text);
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

		// Create Response Buttons
		if (g_CurrentDialogue.response_buttons == NULL) {
			int num = node->num_responses;
			SDL_Rect btn_rect = {
				.x = DIA_BOX_POS_X + TTFTEXT_BOX_BORDER_WIDTH + TTFTEXT_BOX_PADDING,
				.y = DIA_BOX_POS_Y + (DIA_BOX_ROWS-1)*TTFTEXT_GLYPH_H + TTFTEXT_BOX_BORDER_WIDTH + TTFTEXT_BOX_PADDING,
				.w = 0, .h = 0,
			};
			Menel_TextButton btn_buf[DIA_MAX_RESPONSES];

			if (node->responses == NULL || num == 0) {
				num = 1;
				btn_buf[0].state = MENEL_BTN_NORMAL;
				btn_buf[0].bounding_box = btn_rect;
				btn_buf[0].on_highlight = NULL;
				btn_buf[0].on_select = &__CB_GoToNode;
				btn_buf[0].user_data = NULL;
				btn_buf[0].text = DIA_DEF_RESP_EXIT;
			} else if (node->responses != NULL && num == 1) {
				btn_buf[0].state = MENEL_BTN_NORMAL;
				btn_buf[0].bounding_box = btn_rect;
				btn_buf[0].on_highlight = NULL;
				btn_buf[0].on_select = &__CB_GoToNode;
				
				// if the next node is 0 it is the exit node
				if (node->responses[0].next == 0x0000) {
					btn_buf[0].user_data = NULL;
					btn_buf[0].text = DIA_DEF_RESP_EXIT;
				} else {
					btn_buf[0].user_data = &node->responses[0].next;
					btn_buf[0].text = DIA_DEF_RESP_NEXT;
				}
			} else {
				for (int i=0; i<num; i++) {
					btn_buf[i].state = MENEL_BTN_NORMAL;
					btn_buf[i].bounding_box = btn_rect;

					// TODO: Spread them out properly
					btn_rect.x += 100;

					btn_buf[i].on_highlight = NULL;
					btn_buf[i].on_select = &__CB_GoToNode;
					btn_buf[i].user_data = &node->responses[i].next;
					btn_buf[i].text = node->responses[i].text;
				}
			}

			g_CurrentDialogue.response_buttons = Menel_TBtnArr_Create(num, btn_buf);
			//g_CurrentDialogue.response_buttons->sel_index = 0;
		}
	}

	// Draw Responses
	Menel_TBtnArr_Draw(g_CurrentDialogue.response_buttons);
}
