
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <screen.h>
#include <scene.h>

#include "src/ttf_text.h"
#include "src/sound.h"


static bool redraw = true;

static bool show_box = false;
static Uint64 box_tick = 0;
static TTFText_Box box = {
	50, 600, 35, 8, CLR_RESET, 0,
	"\n"
	" Hello, how are you today?\n"
	"\n"
	"     Fine, thanks.\n"
	"     UTF-8 Wörks, btw. §°è£\n"
	"   → Show me your tits! (BAD END)\n"
	"     Goodbye.\n"
};
static int music = 0;
static SDL_Texture *eruya = NULL;


//	Menu Option Callbacks
void scn_title_cb_play() {
}

void scn_title_cb_hello() {
	puts("Hello, world!");
	fflush(stdout);
}

void scn_title_cb_exit() {
	g_isRunning = false;
}


//	Scene Initialisation
void scn_title_setup() {
	Sound_SFX_Prepare(SFX_DIALOGUE_BEEP);
	Sound_OST_QueueTrack(OST_TEST_1);
	Sound_OST_FadeNext(500);

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	eruya = IMG_LoadTexture(g_renderer, "assets/img/eruya.png");
}


//	Scene Termination
void scn_title_teardown() {
	Sound_OST_ClearQueue();
	Sound_OST_FadeNext(500);
	Sound_SFX_ClearAll();

	SDL_DestroyTexture(eruya);
	IMG_Quit();
}


//	Scene Event Handler
void scn_title_handle_events(SDL_Event evt) {
	if (evt.type == SDL_QUIT) g_isRunning = false;

	// DEBUG: Get typed-out text box working
	Uint64 now = SDL_GetTicks64();

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

	if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_F1) {
		if (show_box) {
			show_box = false;
			Mix_FadeOutChannel(0, 100);
		} else {
			show_box = true;
			box_tick = now;
			box.charcount = 1;
		}
	}

	if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_SPACE) {
		if (show_box && box.charcount > 0) {
			box.charcount = -1;
		}
	}
	
	if (box.charcount > 0) {
		int diff = (now - box_tick);
		if (diff > 50) {
			box.charcount++;
			box_tick = now;

			Sound_SFX_Play(SFX_DIALOGUE_BEEP, 0);
		}
	}

	if (box.charcount > box.cols * box.rows) {
		box.charcount = -1;
		Mix_FadeOutChannel(0, 100);
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
		383, 879,
	});

	if (show_box) TTFText_Draw_Box(box);
}


//	Global Scene Handle
Scene scn_title = {
	.setup = &scn_title_setup,
	.teardown = &scn_title_teardown,
	.handle_events = &scn_title_handle_events,
	.draw_frame = &scn_title_draw_frame,
};