
#include <SDL2/SDL.h>
#include <screen.h>
#include <scene.h>

#include "src/menu_element.h"
#include "src/ttf_text.h"
#include "src/pix.h"


static char *next_scene = NULL;
static Menel_TextButtonArray *menu_buttons = NULL;


//	Menu Option Callbacks
static void __cb_dialogue_demo(void *el) {
	next_scene = "dia";
}

static void __cb_sound_demo(void *el) {
	next_scene = "snd";
}

static void __cb_hello(void *el) {
	Log_Message(LOG_WARNING, "Hello, world!");
	puts("Hello, world!");
	fflush(stdout);
}

static void __cb_exit(void *el) {
	g_isRunning = false;
}


//	Scene Initialisation
void scn_title_setup() {
	menu_buttons = Menel_TBtnArr_Create(4, (Menel_TextButton[4]){
		{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 300, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_dialogue_demo,
			.user_data = NULL,
			.text = "Dialogue System Demo", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 350, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_sound_demo,
			.user_data = NULL,
			.text = "Sound System Demo", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 400, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_hello,
			.user_data = NULL,
			.text = "Hellö", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 50, 450, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_exit,
			.user_data = NULL,
			.text = "Göödbyé!", 
		},
	});
	if (menu_buttons == NULL) {
		Log_Message(LOG_ERROR, "Failed to make buttons");
		g_isRunning = false;
	}

	Pix_Load(PIX_TIT_SMILE);
}


//	Scene Termination
void scn_title_teardown() {
	Pix_Clear(PIX_TIT_SMILE);
	Menel_TBtnArr_Destroy(menu_buttons);
}


//	Scene Event Handler
void scn_title_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;

	Menel_TBtnArr_HandleEvent(menu_buttons, evt);

	if (next_scene != NULL) {
		Scene_Set(next_scene);
		next_scene = NULL;
	}
}


//	Scene Draw Calls
void scn_title_draw_frame() {
	Colours_SetRenderer(CLR_WINDOW_BG);
	SDL_RenderClear(g_renderer);

	Pix_Draw(PIX_TIT_SMILE, 50, 50, -1, -1);

	TTFText_Draw_Box((TTFText_Box){
		50, 200,
		25, 1,
		CLR_SPECIAL, -1,
		" Project Bonkhonagahoogs"
	});

	Menel_TBtnArr_Draw(menu_buttons);
}


//	Global Scene Handle
Scene scn_title = {
	.setup = &scn_title_setup,
	.teardown = &scn_title_teardown,
	.handle_events = &scn_title_handle_events,
	.draw_frame = &scn_title_draw_frame,
};