#include "userdata.h"


Datablock_File *g_Userdata_File = NULL;


void UserData_Init() {
	if (g_Userdata_File != NULL) {
		Log_Message(LOG_WARNING, "UserData_Ensure called after UserData is already set. Redundancy?");
		return;
	}

	g_Userdata_File = Datablock_File_Open(UDATA_FILENAME);
	if (g_Userdata_File != NULL) {
		Binding_InitFromFile(g_Userdata_File, UDATA_DBID_BINDINGS);
		Log_Message(LOG_INFO, "Successfully Initialised User-Data System! '" UDATA_FILENAME "' Loaded");
		return;
	}
	Log_Message(LOG_INFO, "No User Data file '" UDATA_FILENAME "' found: Creating...");

	// Create User Data File with default settings
	const int num_blocks = 2;
	Datablock *user_data_blocks[num_blocks];

	// Create Audio settings
	float default_volume = 0.30f;	// Default SFX Volume: 30%
	user_data_blocks[0] = Datablock_Create(UDATA_DBID_AUDIOPRF, &default_volume, sizeof(default_volume)); // SFX Vol
	default_volume = 0.10f;	// Default OST Volume: 10%
	user_data_blocks[1] = Datablock_Create(UDATA_DBID_AUDIOPRF, &default_volume, sizeof(default_volume)); // OST Vol

	// Create Black Colour Palette
	// TODO: How to keep in sync with colours.h?
	// Don't want circular dependency...
	// Make Colour init write palette?
	//const int num_clrs = 3;
	//SDL_Colour default_colour = { 0x00, 0x00, 0x00, 0xFF };
	//user_data_blocks[2] = Datablock_Create(UDATA_DBID_COLOURS, &default_colour, sizeof(default_colour));

	g_Userdata_File = Datablock_File_Create(UDATA_FILENAME, user_data_blocks, num_blocks);

	// Create Keybindings
	Binding_Init();
	Binding_Add(INPUT_UP, SDLK_UP);
	Binding_Add(INPUT_UP, SDLK_w);
	Binding_Add(INPUT_UP, SDLK_k);
	Binding_Add(INPUT_DOWN, SDLK_DOWN);
	Binding_Add(INPUT_DOWN, SDLK_s);
	Binding_Add(INPUT_DOWN, SDLK_j);
	Binding_Add(INPUT_LEFT, SDLK_LEFT);
	Binding_Add(INPUT_LEFT, SDLK_a);
	Binding_Add(INPUT_LEFT, SDLK_h);
	Binding_Add(INPUT_RIGHT, SDLK_RIGHT);
	Binding_Add(INPUT_RIGHT, SDLK_d);
	Binding_Add(INPUT_RIGHT, SDLK_l);
	Binding_Add(INPUT_SELECT, SDLK_RETURN);
	Binding_Add(INPUT_SELECT, SDLK_KP_ENTER);
	Binding_Add(INPUT_SELECT, SDLK_SPACE);
	Binding_Add(INPUT_BACK, SDLK_BACKSPACE);

	Binding_WriteToFile(g_Userdata_File, UDATA_DBID_BINDINGS);

	// Reload file (Bug in current GT version makes this necessary)
	Datablock_File_Close(g_Userdata_File);
	g_Userdata_File = Datablock_File_Open(UDATA_FILENAME);

	Log_Message(LOG_INFO, "Successfully Initialised User-Data System! '" UDATA_FILENAME "' Created");
}

void UserData_Term() {
	Binding_Term();

	// TODO: Write any necessary changes to file

	Datablock_File_Close(g_Userdata_File);
	Log_Message(LOG_INFO, "Terminated User-Data System");
}

//	Internal function to look up datablock indices in the userdata file
//	
//	returns -1 if the block was not found, or the file is not yet loaded
static int __UserData_Lookup(Uint16 type, int num) {
	if (g_Userdata_File == NULL) return -1;

	int n_of_type = 0;
	for (int i=0; i<g_Userdata_File->num_blocks; i++) {
		Datablock_IndexEntry entry = g_Userdata_File->index[i];
		if (entry.block_type != type) continue;
		if (n_of_type == num) return i;
		n_of_type++;
	}

	return -1;
}

int UserData_Get(Uint16 type, int num, void *data, size_t size) {
	int block_index = __UserData_Lookup(type, num);
	if (block_index < 0) {
		char msg[256];
		SDL_snprintf(msg, 256, "Tried to get Block 0x%04X:%02i from User Data; Not Found", type, num);
		Log_Message(LOG_WARNING, msg);
		return -1;
	}

	Datablock *db = Datablock_File_GetBlock(g_Userdata_File, block_index);
	if (data == NULL || size < db->block_length) {
		char msg[256];
		if (data == NULL) {
			SDL_snprintf(msg, 256,
				"Tried to get Block 0x%04X:%02i from User Data; Provided buffer is invalid (NULL-Pointer)",
				type, num
			);
		} else {
			SDL_snprintf(msg, 256,
				"Tried to get Block 0x%04X:%02i from User Data; Provided buffer is invalid (too small: %iB < %iB)",
				type, num, (int) size, (int) db->block_length
			);
		}

		Log_Message(LOG_ERROR, msg);
		Datablock_Destroy(db);
		return -2;
	}

	bool is_valid = Datablock_IsValid(db);
	SDL_memcpy(data, db->data, db->block_length);
	Datablock_Destroy(db);

	if (is_valid) return 1;

	// Loaded successfully, but checksum was invalid
	char msg[256];
	SDL_snprintf(msg, 256, "Read Block 0x%04X:%02i from User Data; Block has invalid checksum", type, num);
	Log_Message(LOG_WARNING, msg);

	return 0;
}

int UserData_Set(Uint16 type, int num, void *data, size_t size) {
	int block_index = __UserData_Lookup(type, num);
	if (block_index < 0) {
		char msg[256];
		SDL_snprintf(msg, 256, "Tried to set Block 0x%04X:%02i of User Data; Not Found", type, num);
		Log_Message(LOG_ERROR, msg);
		return -1;
	}

	Datablock *db = Datablock_File_GetBlock(g_Userdata_File, block_index);
	if (data == NULL || size > db->block_length) {
		char msg[256];
		if (data == NULL) {
			SDL_snprintf(msg, 256,
				"Tried to set Block 0x%04X:%02i of User Data; Provided buffer is invalid (NULL-Pointer)",
				type, num
			);
		} else {
			SDL_snprintf(msg, 256,
				"Tried to set Block 0x%04X:%02i of User Data; Provided buffer is invalid (too big: %iB > %iB)",
				type, num, (int) size, (int) db->block_length
			);
		}

		Log_Message(LOG_ERROR, msg);
		Datablock_Destroy(db);
		return -2;
	}

	// Update Datablock
	SDL_memcpy(db->data, data, size);
	Datablock_CalcSum(db);
	Uint8 chk[4] = {
		(db->checksum >> (3 * 8)) & 0xFF,
		(db->checksum >> (2 * 8)) & 0xFF,
		(db->checksum >> (1 * 8)) & 0xFF,
		(db->checksum >> (0 * 8)) & 0xFF,
	};

	// Write updated datablock to file
	FILE *fud = fopen(g_Userdata_File->filename, "r+b");
	if (fud == NULL) {
		char msg[256];
		SDL_snprintf(msg, 256,
			"Tried to set Block 0x%04X:%02i of User Data; Failed to open '%s' with write permission",
			type, num, g_Userdata_File->filename
		);

		Datablock_Destroy(db);
		return -3;
	}

	long data_pos = g_Userdata_File->index[block_index].file_offset;
	fseek(fud, data_pos + sizeof(Uint32), SEEK_SET);
	fwrite(db->data, sizeof(Uint8), db->block_length, fud);
	fwrite(chk, sizeof(Uint8), 4, fud);
	fflush(fud);
	fclose(fud);

	Datablock_Destroy(db);
	return 0;
}

// TODO:
// Implement this if the engine gets to the point of
// completion that it needs customizable keybinds
void UserData_SetBinding(Input_Type in, SDL_KeyCode kc) {
	if (g_keybinds == NULL) return;
	if (g_Userdata_File == NULL) return;
	if (in == INPUT_NONE) return;
	if (kc == SDLK_UNKNOWN) return;

	// Check if this keycode is already in use
	//int prev = Binding_GetByKeyCode(kc);
	//if (prev >= 0) {
	//	if (g_keybinds[prev].num_keycodes == 1) {
	//		g_keybinds[prev].input = INPUT_NONE;
	//		g_keybinds[prev].is_held = false;
	//		g_keybinds[prev].num_keycodes = 0;
	//		g_keybinds[prev].keycodes[0] = SDLK_UNKNOWN;
	//	}

	//	for (int i=0; i<g_keybinds[prev].num_keycodes; i++) {
	//		if (g_keybinds[prev].keycodes[i]
	//	}
	//	g_keybinds[prev].input
	//}

	//Binding_WriteToFile(g_Userdata_File, )

	//for (int i=0; i<g_keybinds; i++) {
	//	if (g_keybinds[i].input == INPUT_NONE) continue;

	//	if (g_keybinds[i].)
	//}
}