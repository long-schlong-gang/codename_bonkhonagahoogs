
#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>

#include "src/menu_element.h"
#include "src/gamestate.h"
#include "src/dialogue.h"
#include "src/ttf_text.h"
#include "src/sound.h"
#include "src/pix.h"

#define MENU_TITLE		(Uint8)(0)
#define MENU_INGAME		(Uint8)(1)
#define MENU_LOAD_GAME	(Uint8)(2)
#define MENU_OPTIONS	(Uint8)(3)

#define MENU_NO_ACTION		(Uint8)(0)
#define MENU_START_GAME		(Uint8)(1)
#define MENU_QUIT_TO_TITLE	(Uint8)(2)

#define OPT_VOL_DELTA 5

static int action = MENU_NO_ACTION;
static int menu_page = MENU_TITLE;
static Menel_TextButtonArray *title_buttons = NULL;
static Menel_TextButtonArray *option_buttons = NULL;
static Menel_TextButtonArray *load_game_buttons = NULL;
static Menel_TextButtonArray *loaded_menu_buttons = NULL; // The variant title buttons to show once the game has loaded
static SDL_TimerID music_timer_id = 0;
static int menu_root_page = MENU_TITLE;
static TTFText_Box popup = {
	.x = 0, .y = 0,
	.cols = 10, .rows = 1,
	.charcount = -1,
	.clr = CLR_BTN_SELECT,
	.str = NULL,
};

static char save_slot_text[3][32];


static void __cb_exit(void *_) {
	g_isRunning = false;
}


// Timer callback to switch music from intro to loop
static Uint32 __cb_music_intro_to_loop(Uint32 interval, void *_) {
	Sound_OST_FadeNext(0);
	return 0;
}

//	Pointer fuckery ahead;
//	If there's a bug in the menu, it's probably this:
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

static void __cb_set_menu_page(void *_page) {
	menu_page = (Uint8)(_page);
	Sound_SFX_Play(SFX_DIALOGUE_BEEP, -1);
}

// Returns to base menu (changes based on whether in-game or not)
static void __cb_back(void *_) {
	menu_page = menu_root_page;
	Sound_SFX_Play(SFX_DIALOGUE_BEEP, -1);
}

static void __cb_set_menu_action(void *_action) {
	action = (Uint8)(_action);
	Sound_SFX_Play(SFX_FIFTH, -1);
}

static void __cb_change_sfx_vol(void *_value) {
	int change = (int)(_value);
	Sound_SFX_ChangeVolume(change * 0.01f);
	Sound_SFX_Play(SFX_TEST_1, -1);
}

static void __cb_change_ost_vol(void *_value) {
	int change = (int)(_value);
	Sound_OST_ChangeVolume(change * 0.01f);
}

static void __cb_load_save(void *_slotnr) {
	Game_Slot slot = (Game_Slot)(_slotnr);
	Gamestate_Load(slot);
	action = MENU_START_GAME;
	Sound_SFX_Play(SFX_FIFTH, -1);
}

static void __cb_save_game(void *_slotnr) {
	Game_Slot slot = (Game_Slot)(_slotnr);
	Gamestate_Save(slot);
	popup.str = "\n Game Saved!";
	popup.cols = 13;
	popup.rows = 3;
	Sound_SFX_Play(SFX_DIT_UP, -1);
}

#pragma GCC diagnostic pop


//	Scene Initialisation
void scn_title_setup() {
	title_buttons = Menel_TBtnArr_Create(3, (Menel_TextButton[3]){
		{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 200, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_set_menu_page,
			.user_data = (void *)(MENU_LOAD_GAME),
			.text = "Start Game", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 250, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_set_menu_page,
			.user_data = (void *)(MENU_OPTIONS),
			.text = "Options", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 500, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_exit,
			.user_data = NULL,
			.text = "Exit", 
		},
	});
	if (title_buttons == NULL) {
		Log_Message(LOG_ERROR, "Failed to create Title-Page buttons");
		g_isRunning = false;
	}

	loaded_menu_buttons = Menel_TBtnArr_Create(4, (Menel_TextButton[4]){
		{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 200, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_set_menu_action,
			.user_data = (void *)(MENU_START_GAME),
			.text = "Back to game", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 250, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_set_menu_page,
			.user_data = (void *)(MENU_OPTIONS),
			.text = "Options", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 300, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_save_game,
			.user_data = (void *)(g_CurrentGame.curr_slot),
			.text = "Save Game", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 500, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_set_menu_action,
			.user_data = (void *)(MENU_QUIT_TO_TITLE),
			.text = "Quit to Title",
		},
	});
	if (title_buttons == NULL) {
		Log_Message(LOG_ERROR, "Failed to create In-Game Menu buttons");
		g_isRunning = false;
	}


	option_buttons = Menel_TBtnArr_Create(5, (Menel_TextButton[5]){
		{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 60, 252, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_change_sfx_vol,
			.user_data = (void *)(-OPT_VOL_DELTA),
			.text = " - ", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 160, 252, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_change_sfx_vol,
			.user_data = (void *)(OPT_VOL_DELTA),
			.text = " + ", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 60, 402, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_change_ost_vol,
			.user_data = (void *)(-OPT_VOL_DELTA),
			.text = " - ", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 160, 402, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_change_ost_vol,
			.user_data = (void *)(OPT_VOL_DELTA),
			.text = " + ", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 500, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_back,
			.user_data = NULL,
			.text = "Back", 
		},
	});
	if (option_buttons == NULL) {
		Log_Message(LOG_ERROR, "Failed to create Option-Page buttons");
		g_isRunning = false;
	}

	Game_SlotInfo slot_1_info = Gamestate_SlotInfo(GAME_SLOT_1);
	Game_SlotInfo slot_2_info = Gamestate_SlotInfo(GAME_SLOT_2);
	Game_SlotInfo slot_3_info = Gamestate_SlotInfo(GAME_SLOT_3);
	
	int hrs, min, sec;

	if (Gamestate_SlotEmpty(GAME_SLOT_1)) {
		SDL_snprintf(save_slot_text[0], 32, "%12s - %02i:%02i:%02i", "Empty Slot", 0, 0, 0);
	} else {
		sec = slot_1_info.playtime;
		min = sec / 60; sec -= min * 60;
		hrs = min / 60; min -= hrs * 60;
		SDL_snprintf(save_slot_text[0], 32, "%12s - %02i:%02i:%02i", slot_1_info.name, hrs, min, sec);
	}

	if (Gamestate_SlotEmpty(GAME_SLOT_2)) {
		SDL_snprintf(save_slot_text[1], 32, "%12s - %02i:%02i:%02i", "Empty Slot", 0, 0, 0);
	} else {
		sec = slot_2_info.playtime;
		min = sec / 60; sec -= min * 60;
		hrs = min / 60; min -= hrs * 60;
		SDL_snprintf(save_slot_text[1], 32, "%12s - %02i:%02i:%02i", slot_2_info.name, hrs, min, sec);
	}

	if (Gamestate_SlotEmpty(GAME_SLOT_3)) {
		SDL_snprintf(save_slot_text[2], 32, "%12s - %02i:%02i:%02i", "Empty Slot", 0, 0, 0);
	} else {
		sec = slot_3_info.playtime;
		min = sec / 60; sec -= min * 60;
		hrs = min / 60; min -= hrs * 60;
		SDL_snprintf(save_slot_text[2], 32, "%12s - %02i:%02i:%02i", slot_3_info.name, hrs, min, sec);
	}

	load_game_buttons = Menel_TBtnArr_Create(4, (Menel_TextButton[4]){
		{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 200, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_load_save,
			.user_data = (void *)(GAME_SLOT_1),
			.text = save_slot_text[0], 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 250, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_load_save,
			.user_data = (void *)(GAME_SLOT_2),
			.text = save_slot_text[1], 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 300, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_load_save,
			.user_data = (void *)(GAME_SLOT_3),
			.text = save_slot_text[2], 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 500, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_back,
			.user_data = NULL,
			.text = "Back", 
		},
	});
	if (load_game_buttons == NULL) {
		Log_Message(LOG_ERROR, "Failed to create Load Game buttons");
		g_isRunning = false;
	}

	popup.x = g_screen_width/2 - (15*TTFText_GlyphWidth())/2 - TTFTEXT_BOX_BORDER_WIDTH - TTFTEXT_BOX_PADDING;
	popup.y = g_screen_height/2 - (3*TTFText_GlyphHeight())/2 - TTFTEXT_BOX_BORDER_WIDTH - TTFTEXT_BOX_PADDING;

	Sound_OST_QueueTrack(OST_TITLE_INTRO);
	Sound_OST_FadeNext(1000);
	Sound_OST_QueueTrack(OST_TITLE_LOOP);

	Uint32 intro_time_ms = Mix_MusicDuration(g_CurrentMusic) * 1000.0f;
	music_timer_id = SDL_AddTimer(intro_time_ms, &__cb_music_intro_to_loop, NULL);

	Sound_SFX_Prepare(SFX_DIALOGUE_BEEP);
	Sound_SFX_Prepare(SFX_TEST_1);
	Sound_SFX_Prepare(SFX_DIT_UP);
	Sound_SFX_Prepare(SFX_FIFTH);

	Pix_Load(PIX_TITLE_SPLASH);
}


//	Scene Termination
void scn_title_teardown() {
	Pix_Clear(PIX_TITLE_SPLASH);

	SDL_RemoveTimer(music_timer_id);

	Sound_OST_ClearQueue();
	Sound_OST_FadeNext(500);

	Sound_SFX_Clear(SFX_DIALOGUE_BEEP);
	Sound_SFX_Clear(SFX_TEST_1);
	Sound_SFX_Clear(SFX_DIT_UP);
	Sound_SFX_Clear(SFX_FIFTH);

	Menel_TBtnArr_Destroy(title_buttons);
	Menel_TBtnArr_Destroy(option_buttons);
}


//	Scene Event Handler
void scn_title_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;

	if (popup.str != NULL) {
		if (evt.type == SDL_KEYUP || evt.type == SDL_MOUSEBUTTONDOWN) {
			popup.str = NULL;
		}
		return;
	}

	switch (menu_page) {
		case MENU_TITLE: Menel_TBtnArr_HandleEvent(title_buttons, evt); break;
		case MENU_INGAME: Menel_TBtnArr_HandleEvent(loaded_menu_buttons, evt); break;
		case MENU_OPTIONS: Menel_TBtnArr_HandleEvent(option_buttons, evt); break;
		case MENU_LOAD_GAME: Menel_TBtnArr_HandleEvent(load_game_buttons, evt); break;
	}

	switch (action) {
		case MENU_START_GAME: {
			menu_page = MENU_INGAME;
			menu_root_page = MENU_INGAME;
			Scene_Set("world");
		} break;
		case MENU_QUIT_TO_TITLE: {
			menu_page = MENU_TITLE;
			menu_root_page = MENU_TITLE;
		} break;
	}
	action = MENU_NO_ACTION;

	if (evt.type == SDL_KEYUP && evt.key.keysym.sym == SDLK_ESCAPE) {
		if (menu_page == MENU_INGAME) Scene_Set("world");
	}
}


//	Scene Draw Calls
void scn_title_draw_frame() {
	Colours_SetRenderer(CLR_WINDOW_BG);
	SDL_RenderClear(g_renderer);

	int splash_sz = g_screen_height - 100;
	Pix_Draw(PIX_TITLE_SPLASH,
		g_screen_width - 50 - splash_sz,
		g_screen_height - 50 - splash_sz,
		splash_sz, splash_sz
	);

	TTFText_Draw_Box((TTFText_Box){
		50, 50,
		16, 1,
		CLR_SPECIAL, -1,
		" Astral Fermata"
	});

	switch (menu_page) {

		case MENU_TITLE: {
			Menel_TBtnArr_Draw(title_buttons);
		} break;

		case MENU_INGAME: {
			Menel_TBtnArr_Draw(loaded_menu_buttons);
		} break;

		case MENU_OPTIONS: {
			Menel_TBtnArr_Draw(option_buttons);
			TTFText_RenderText(
				50 + MENEL_TXTBTN_OUTLINE + MENEL_TXTBTN_PADDING,
				200 + MENEL_TXTBTN_OUTLINE + MENEL_TXTBTN_PADDING,
				CLR_TEXT_NORM, "SFX Volume:"
			);
			TTFText_RenderText(
				50 + MENEL_TXTBTN_OUTLINE + MENEL_TXTBTN_PADDING,
				350 + MENEL_TXTBTN_OUTLINE + MENEL_TXTBTN_PADDING,
				CLR_TEXT_NORM, "OST Volume:"
			);

			int max_width = 400;
			char vol_buf[6];
			Colours_SetRenderer(CLR_TEXT_NORM);

			// Render SFX Volume percent
			float vol = (float) Mix_MasterVolume(-1) / MIX_MAX_VOLUME;
			SDL_RenderDrawRect(g_renderer, &(struct SDL_Rect){
				250, 250,
				max_width + 4, 50 + 4
			});
			SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
				250 + 2, 250 + 2,
				(int)(vol * (float) max_width), 50
			});
			int round_vol = (int)(vol * 100);
			if (round_vol > 0 && round_vol < OPT_VOL_DELTA) round_vol = OPT_VOL_DELTA;
			else round_vol -= round_vol % OPT_VOL_DELTA;
			SDL_snprintf(vol_buf, 6, "%i%%", round_vol);
			TTFText_RenderText(250 + max_width + 50, 255, CLR_TEXT_NORM, vol_buf);

			// Render OST Volume percent
			vol = (float) Mix_VolumeMusic(-1) / MIX_MAX_VOLUME;
			SDL_RenderDrawRect(g_renderer, &(struct SDL_Rect){
				250, 400,
				max_width + 4, 50 + 4
			});
			SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
				250 + 2, 400 + 2,
				(int)(vol * (float) max_width), 50
			});

			round_vol = (int)(vol * 100);
			if (round_vol > 0 && round_vol < OPT_VOL_DELTA) round_vol = OPT_VOL_DELTA;
			else round_vol -= round_vol % OPT_VOL_DELTA;
			SDL_snprintf(vol_buf, 6, "%i%%", round_vol);
			TTFText_RenderText(250 + max_width + 50, 405, CLR_TEXT_NORM, vol_buf);

		} break;

		case MENU_LOAD_GAME: {
			Menel_TBtnArr_Draw(load_game_buttons);
		} break;
	}

	if (popup.str != NULL) TTFText_Draw_Box(popup);
}


//	Global Scene Handle
Scene scn_title = {
	.setup = &scn_title_setup,
	.teardown = &scn_title_teardown,
	.handle_events = &scn_title_handle_events,
	.draw_frame = &scn_title_draw_frame,
};