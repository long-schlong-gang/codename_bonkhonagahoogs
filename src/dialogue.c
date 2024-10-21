#include "dialogue.h"

Dialogue_Tree g_CurrentDialogue = {
	//.root = NULL,
	.root_id = NULL_NODE,
	.current = NULL,
	.next_node = -1,
	.response_buttons = NULL,
	.npc_poses = { POSE_NOT_PRESENT, POSE_NOT_PRESENT, POSE_NOT_PRESENT, POSE_NOT_PRESENT },
	.bg_music = OST_NONE,
	.background = -1,
};


static Image __npc_poses[4][4] = {
	{	// Levu
		PIX_LEVU_NEUTRAL,
		PIX_LEVU_HAPPY,
		PIX_LEVU_NEUTRAL,
		PIX_LEVU_ANGRY,
	},
	{	// Eruya
		PIX_ERUYA_NEUTRAL,
		PIX_ERUYA_HAPPY,
		PIX_ERUYA_NEUTRAL,
		PIX_ERUYA_ANGRY,
	},
	{	// Fedelov
		PIX_FEDELOV_NEUTRAL,
		PIX_FEDELOV_HAPPY,
		PIX_FEDELOV_NEUTRAL,
		PIX_FEDELOV_ANGRY,
	},
	{	// Kelen
		PIX_TIT_SMILE,
		PIX_TIT_SMILE,
		PIX_TIT_SMILE,
		PIX_TIT_SMILE,
	},
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

	Uint16 posetbl = 0x0000;
	U8_TO_U16(posetbl, *(Uint8 *)(data), *(Uint8 *)(data+1));
	data += sizeof(posetbl);

	for (int i=0; i<4; i++) {
		Dialogue_Pose pose = (posetbl >> ((3-i)*4)) & 0b1111;
		g_CurrentDialogue.npc_poses[i] = pose;
		if (pose == POSE_NOT_PRESENT) continue;
		Image img = __npc_poses[i][pose - 1];
		Pix_Load(img);
	}

	Uint16 snd_id;
	SDL_memcpy(&snd_id, data, sizeof(snd_id));
	data += sizeof(snd_id);
	g_CurrentDialogue.bg_music = snd_id;
	if (snd_id > 0) Sound_OST_QueueTrack(snd_id);

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

	if (g_CurrentDialogue.bg_music != OST_NONE) {
		Sound_OST_ClearQueue();
		Sound_OST_FadeNext(100);
		g_CurrentDialogue.bg_music = OST_NONE;
	}

	for (int i=0; i<4; i++) {
		Dialogue_Pose pose = g_CurrentDialogue.npc_poses[i];
		if (pose == POSE_NOT_PRESENT) continue;
		Image img = __npc_poses[i][pose - 1];
		Pix_Load(img);
	}

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

	if (g_CurrentDialogue.bg_music != OST_NONE) {
		Sound_OST_QueueTrack(g_CurrentDialogue.bg_music);
		Sound_OST_FadeNext(100);
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

	if (g_CurrentDialogue.background >= 0) {
		Pix_Draw(g_CurrentDialogue.background,
			25, 25,
			g_screen_width - 50,
			g_screen_height - 50
		);
	}

	for (int i=0; i<4; i++) {
		Dialogue_Pose pose = g_CurrentDialogue.npc_poses[i];
		if (pose == POSE_NOT_PRESENT) continue;
		Image img = __npc_poses[i][pose - 1];
		Pix_Draw(img,
			DIA_NPC_POS_X + i * DIA_NPC_OFFS,
			DIA_NPC_POS_Y,
			-1, -1
		);
	}

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
		char msg[256];
		SDL_snprintf(msg, 256,
			"Dialogue Node (0x%04X, '%s') has an invalid checksum",
			dbid, __curr_dia_file->filename
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
		node->responses = SDL_malloc(sizeof(Dialogue_Response) * node->num_responses);
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

	TTFText_Box dia_box = {
		DIA_BOX_PADDING,
		g_screen_height - DIA_BOX_PADDING - (DIA_BOX_ROWS * TTFText_GlyphHeight()),
		DIA_BOX_COLS, DIA_BOX_ROWS,
		CLR_TEXT_NORM, node->state,
		node->text
	};
	int written = TTFText_Draw_Box(dia_box);

	if (written < 0) {
		node->state = -1;
		Sound_SFX_Clear(SFX_DIALOGUE_BEEP);

		// Create Response Buttons
		if (g_CurrentDialogue.response_buttons == NULL) {
			int num = node->num_responses;
			int padd = TTFTEXT_BOX_BORDER_WIDTH - (MENEL_TXTBTN_OUTLINE + MENEL_TXTBTN_PADDING);
			SDL_Rect btn_rect = {
				.x = dia_box.x + padd + 2*TTFTEXT_BOX_PADDING,
				.y = dia_box.y + (DIA_BOX_ROWS-1)*TTFText_GlyphHeight() + padd,
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

					if (node->responses[0].text == NULL
						|| node->responses[0].text[0] == '\0'
					) { 
						btn_buf[0].text = DIA_DEF_RESP_EXIT;
					} else {
						btn_buf[0].text = node->responses[0].text;
					}
				} else {
					btn_buf[0].user_data = &node->responses[0].next;

					if (node->responses[0].text == NULL
						|| node->responses[0].text[0] == '\0'
					) { 
						btn_buf[0].text = DIA_DEF_RESP_NEXT;
					} else {
						btn_buf[0].text = node->responses[0].text;
					}
				}
			} else {
				for (int i=0; i<num; i++) {
					btn_buf[i].state = MENEL_BTN_NORMAL;
					btn_buf[i].bounding_box = btn_rect;
					btn_buf[i].on_highlight = NULL;
					btn_buf[i].on_select = &__CB_GoToNode;
					btn_buf[i].user_data = &node->responses[i].next;
					btn_buf[i].text = node->responses[i].text;

					int x_offset = (SDL_utf8strlen(node->responses[i].text) + 1) * TTFText_GlyphWidth();
					btn_rect.x += x_offset + MENEL_TXTBTN_PADDING + MENEL_TXTBTN_OUTLINE;
				}
			}

			g_CurrentDialogue.response_buttons = Menel_TBtnArr_Create(num, btn_buf);
			//g_CurrentDialogue.response_buttons->sel_index = 0;
		}
	}

	// Draw Responses
	Menel_TBtnArr_Draw(g_CurrentDialogue.response_buttons);
}
