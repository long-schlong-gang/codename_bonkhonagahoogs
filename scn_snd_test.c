
#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>

#include "src/menu_element.h"
#include "src/ttf_text.h"
#include "src/sound.h"

static Menel_TextButtonArray *sndtest_buttons = NULL;
static bool go_back = false;

static Sound_Music track_1 = OST_TEST_1;
static Sound_Music track_2 = OST_TEST_2;
static Sound_Music track_3 = OST_TEST_3;
static Sound_Effect sfx_1 = SFX_DIALOGUE_BEEP;
static Sound_Effect sfx_2 = SFX_TEST_1;


//	Menu Button Callbacks
static void __cb_sound_pause(void *el) {
	while (Mix_FadingMusic() != MIX_NO_FADING) SDL_Delay(5);
	Sound_OST_TogglePause(0);
}

static void __cb_set_ost(void *_ost) {
	if (_ost == NULL) return;
	Sound_Music ost = *(Sound_Music *)(_ost);
	Sound_OST_QueueTrack(ost);
	Sound_OST_FadeNext(500);
}

static void __cb_play_sfx(void *_sfx) {
	if (_sfx == NULL) return;
	Sound_Effect sfx = *(Sound_Effect *)(_sfx);
	Sound_SFX_Play(sfx, -1);
}

static void __cb_go_back(void *el) {
	go_back = true;
}

//	Scene Initialisation
void scn_soundtest_setup() {
	sndtest_buttons = Menel_TBtnArr_Create(7, (Menel_TextButton[7]){
		{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 200, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_sound_pause,
			.user_data = NULL,
			.text = "Pause Music", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 250, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_set_ost,
			.user_data = &track_1,
			.text = "Track 1", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 300, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_set_ost,
			.user_data = &track_2,
			.text = "Track 2", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 350, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_set_ost,
			.user_data = &track_3,
			.text = "Track 3", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 500, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_play_sfx,
			.user_data = &sfx_1,
			.text = "SFX 1", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 550, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_play_sfx,
			.user_data = &sfx_2,
			.text = "SFX 2", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 800, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_go_back,
			.user_data = NULL,
			.text = "Back to Title", 
		},
	});
	Sound_SFX_Prepare(SFX_DIALOGUE_BEEP);
	Sound_SFX_Prepare(SFX_TEST_1);

	if (sndtest_buttons == NULL) {
		Log_Message(LOG_ERROR, "Failed to make buttons");
		g_isRunning = false;
	}

	Mix_MasterVolume(20);
	Mix_VolumeMusic(20);

	Sound_OST_QueueTrack(OST_TEST_1);
	Sound_OST_FadeNext(1000);
}


//	Scene Termination
void scn_soundtest_teardown() {
	Sound_SFX_ClearAll();
	Sound_OST_ClearQueue();
	Sound_OST_FadeNext(1000);
}


//	Scene Event Handler
void scn_soundtest_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;
	
	Menel_TBtnArr_HandleEvent(sndtest_buttons, evt);

	if (go_back == true) {
		Scene_Set("title");
		go_back = false;
	}
}


//	Scene Draw Calls
void scn_soundtest_draw_frame() {
	Colours_SetRenderer(CLR_WINDOW_BG);
	SDL_RenderClear(g_renderer);

	TTFText_Draw_Box((TTFText_Box){
		50, 50,
		16, 1,
		CLR_SPECIAL, -1,
		"Sound Test Menu"
	});

	char msg[256];
	int vol = ((float) Mix_VolumeMusic(-1) / MIX_MAX_VOLUME) * 100;
	SDL_snprintf(msg, 256, "Volume: %i%%", vol);
	TTFText_RenderText(700, 50, CLR_TEXT_EMPH, msg);

	Menel_TBtnArr_Draw(sndtest_buttons);
}


//	Global Scene Handle
Scene scn_soundtest = {
	.setup = &scn_soundtest_setup,
	.teardown = &scn_soundtest_teardown,
	.handle_events = &scn_soundtest_handle_events,
	.draw_frame = &scn_soundtest_draw_frame,
};