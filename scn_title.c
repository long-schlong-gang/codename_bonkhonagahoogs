
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <screen.h>
#include <scene.h>

#include "src/menu_element.h"
#include "src/ttf_text.h"
#include "src/dialogue.h"
#include "src/sound.h"


static bool redraw = true;

static int music = 0;
static SDL_Texture *eruya = NULL;
static Menel_TextButtonArray *buttons = NULL;


//	Menu Option Callbacks
static void __cb_pause_music(void *el) {
	Sound_OST_TogglePause(50);
}

static void __cb_hello(void *el) {
	puts("Hello, world!");
	fflush(stdout);
}

static void __cb_exit(void *el) {
	g_isRunning = false;
}


//	Scene Initialisation
void scn_title_setup() {
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	eruya = IMG_LoadTexture(g_renderer, "assets/img/eruya.jpg");

	buttons = Menel_TBtnArr_Create(3, (Menel_TextButton[3]){
		{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 400, 50, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_hello,
			.text = "Hellö", 
		},{
			.state = MENEL_BTN_DISABLED,
			.bounding_box = { 400, 150, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_pause_music,
			.text = "Pause Music", 
		},{
			.state = MENEL_BTN_NORMAL,
			.bounding_box = { 400, 250, 0, 0 },
			.on_highlight = NULL,
			.on_select = &__cb_exit,
			.text = "Göödbyé!", 
		},
	});
	if (buttons == NULL) {
		Log_Message(LOG_ERROR, "Failed to make buttons");
		g_isRunning = false;
	}

	//Dialogue_LoadTree(DIALOGUE_FILENAME);

	Sound_SFX_Prepare(SFX_DIALOGUE_BEEP);
	//Sound_OST_QueueTrack(OST_TEST_1);
	//Sound_OST_FadeNext(500);
}


//	Scene Termination
void scn_title_teardown() {
	Sound_OST_ClearQueue();
	Sound_OST_FadeNext(500);
	Sound_SFX_ClearAll();

	//Dialogue_UnloadTree();

	Menel_TBtnArr_Destroy(buttons);

	SDL_DestroyTexture(eruya);
	IMG_Quit();
}


//	Scene Event Handler
void scn_title_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;
	//Binding_HandleEvent(evt);

	//Dialogue_HandleEvents(evt);

	Menel_TBtnArr_HandleEvent(buttons, evt);

	if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_q) {
		Sound_OST_ClearQueue();
		Sound_OST_FadeNext(1000);
	}

	if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_F2) {
		if (music == 0) {
			music = 1;
			Sound_OST_QueueTrack(OST_TEST_2);
		} else if (music == 1) {
			music = 2;
			Sound_OST_QueueTrack(OST_TEST_3);
		} else if (music == 2) {
			music = 0;
			Sound_OST_QueueTrack(OST_TEST_1);
		}

		Sound_OST_FadeNext(2500);
	}

	redraw = true;
}


//	Scene Draw Calls
void scn_title_draw_frame() {
	if (!redraw) return;

	Colours_SetRenderer(CLR_WINDOW_BG);
	SDL_RenderClear(g_renderer);

	//TTFText_RenderGlyph(10, 10, CLR_SPECIAL, 0x00FE); // Thorn
	//TTFText_RenderText(50, 50, 26, "Héllö, würld!");

	SDL_RenderCopy(g_renderer, eruya, NULL, &(struct SDL_Rect){
		50, 50,
		339, 859,
	});

	Menel_TBtnArr_Draw(buttons);

	//Dialogue_DrawAll();
}


//	Global Scene Handle
Scene scn_title = {
	.setup = &scn_title_setup,
	.teardown = &scn_title_teardown,
	.handle_events = &scn_title_handle_events,
	.draw_frame = &scn_title_draw_frame,
};

//static void __cb_debug(void *el) {
//	puts("Button States:");
//	printf("  btn_hello: [%s]\n", (btn_hello->state == 0) ? "NORMAL" : ((btn_hello->state == 1) ? "DISABLED" : ((btn_hello->state == 2) ? "HIGHLIGHTED" : "SELECTED" )) );
//	printf("  btn_pause: [%s]\n", (btn_pause->state == 0) ? "NORMAL" : ((btn_pause->state == 1) ? "DISABLED" : ((btn_pause->state == 2) ? "HIGHLIGHTED" : "SELECTED" )) );
//	printf("  btn_exit: [%s]\n", (btn_exit->state == 0) ? "NORMAL" : ((btn_exit->state == 1) ? "DISABLED" : ((btn_exit->state == 2) ? "HIGHLIGHTED" : "SELECTED" )) );
//	fflush(stdout);
//}
