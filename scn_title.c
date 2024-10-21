
#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>

#include "src/menu_element.h"
#include "src/gamestate.h"
#include "src/ttf_text.h"
#include "src/sound.h"
#include "src/pix.h"

#define MENU_TITLE		(Uint8)(0)
#define MENU_NEW_GAME	(Uint8)(1)
#define MENU_LOAD_GAME	(Uint8)(2)
#define MENU_OPTIONS	(Uint8)(3)

#define MENU_NO_ACTION	(Uint8)(0)
#define MENU_START_GAME	(Uint8)(1)

#define OPT_VOL_DELTA 5

static int action = MENU_NO_ACTION;
static int menu_page = MENU_TITLE;
static Menel_TextButtonArray *title_buttons = NULL;
static Menel_TextButtonArray *option_buttons = NULL;
//static Menel_TextButtonArray *load_game_buttons = NULL;
//static Menel_TextButtonArray *new_game_buttons = NULL;

// Timer stuff to handle moving from title theme intro to loop
static int intro_remaining_ms = -1;
static Uint64 intro_time = 0;


//	Menu Option Callbacks
static void __cb_start_game(void *_) {
	action = MENU_START_GAME;
}

static void __cb_exit(void *_) {
	g_isRunning = false;
}


//	Pointer fuckery ahead;
//	If there's a bug in the menu, it's probably this:
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

static void __cb_set_menu_page(void *_page) {
	menu_page = (Uint8)(_page);
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

#pragma GCC diagnostic pop


//	Scene Initialisation
void scn_title_setup() {
	title_buttons = Menel_TBtnArr_Create(4, (Menel_TextButton[4]){
		{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 200, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_start_game,
			.user_data = NULL,
			.text = "New Game", 
		},{
			.state = MENEL_BTN_DISABLED,
			.bounding_box = { 50, 250, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_set_menu_page,
			.user_data = (void *)(MENU_LOAD_GAME),
			.text = "Load Game", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 300, 0, 0 },
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
			.on_select = &__cb_set_menu_page,
			.user_data = MENU_TITLE,
			.text = "Back", 
		},
	});
	if (option_buttons == NULL) {
		Log_Message(LOG_ERROR, "Failed to create Option-Page buttons");
		g_isRunning = false;
	}

	// TODO: User Saves
	//load_game_buttons = Menel_TBtnArr_Create(4, (Menel_TextButton[4]){
	//	{
	//		.state = MENEL_BTN_DISABLED,
	//		.bounding_box = { 50, 200, 0, 0 },
	//		.on_highlight = NULL,
	//		.on_select = NULL,
	//		.user_data = NULL,
	//		.text = "Save 1 - NAME", 
	//	},{
	//		.state = MENEL_BTN_NORMAL,
	//		.bounding_box = { 50, 250, 0, 0 },
	//		.on_highlight = NULL,
	//		.on_select = NULL,
	//		.user_data = NULL,
	//		.text = "Save 2 - NAME", 
	//	},{
	//		.state = MENEL_BTN_NORMAL,
	//		.bounding_box = { 50, 300, 0, 0 },
	//		.on_highlight = NULL,
	//		.on_select = NULL,
	//		.user_data = NULL,
	//		.text = "Save 3 - NAME", 
	//	},{
	//		.state = MENEL_BTN_NORMAL,
	//		.bounding_box = { 50, 500, 0, 0 },
	//		.on_highlight = NULL,
	//		.on_select = &__cb_set_menu_page,
	//		.user_data = MENU_TITLE,
	//		.text = "Back", 
	//	},
	//});
	//if (load_game_buttons == NULL) {
	//	Log_Message(LOG_ERROR, "Failed to create Option-Page buttons");
	//	g_isRunning = false;
	//}

	if (intro_remaining_ms < 0) {
		Sound_OST_QueueTrack(OST_TITLE_INTRO);
		intro_time = SDL_GetTicks64();
		Sound_OST_FadeNext(1000);
		intro_remaining_ms = Mix_MusicDuration(g_CurrentMusic) * 1000;
		Sound_OST_QueueTrack(OST_TITLE_LOOP);
	}

	Sound_SFX_Prepare(SFX_TEST_1);

	Pix_Load(PIX_TITLE_SPLASH);
}


//	Scene Termination
void scn_title_teardown() {
	Pix_Clear(PIX_TITLE_SPLASH);

	Sound_SFX_Clear(SFX_TEST_1);

	Sound_OST_ClearQueue();
	Sound_OST_FadeNext(1000);

	Menel_TBtnArr_Destroy(title_buttons);
	Menel_TBtnArr_Destroy(option_buttons);
}


//	Scene Event Handler
void scn_title_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;

	if (intro_remaining_ms > 0) {
		Uint64 now = SDL_GetTicks64();
		int elapsed = now - intro_time;
		intro_remaining_ms -= elapsed;
		intro_time = now;
		if (intro_remaining_ms < 0) {
			intro_remaining_ms = 0;
			Sound_OST_FadeNext(0);
		}
	}

	switch (menu_page) {
		case MENU_TITLE: Menel_TBtnArr_HandleEvent(title_buttons, evt); break;
		case MENU_OPTIONS: Menel_TBtnArr_HandleEvent(option_buttons, evt); break;
	}


	switch (action) {
		case MENU_START_GAME: Gamestate_NewGame(); action = MENU_NO_ACTION; break;
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

	}

	//	Le Rainbow Bar :P
	//	
	//Colours_SetRenderer(CLR_SPECIAL);
	//SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
	//	50, g_screen_height - 50 - 50,
	//	intro_remaining_ms, 50
	//});
}


//	Global Scene Handle
Scene scn_title = {
	.setup = &scn_title_setup,
	.teardown = &scn_title_teardown,
	.handle_events = &scn_title_handle_events,
	.draw_frame = &scn_title_draw_frame,
};