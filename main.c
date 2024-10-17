//	
//	PROJECT BONKHONAGAHOOGS
//	
//	Source Code v0.2.0
//	By Olorin
//	

// TODO: Change Engine/Game Name
#define ENGINE "Proj. BHGH"
#define VERSION "0.2.0"
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 1024

#include <SDL2/SDL.h>

#include <log.h>
#include <util.h>
#include <screen.h>
#include <events.h>

#include "src/userdata.h"
#include "src/ttf_text.h"
#include "src/sound.h"
#include "src/pix.h"

//	Include Scenes
#include "scn_title.c"
#include "scn_dialogue.c"
#include "scn_snd_test.c"


int main(int argc, char* args[]) {
	// Set Logging to Develop Mode
	// TODO: Change for Prod
	Log_SetPrintLevel(LOG_DEBUG);
	Log_SetPopupLevel(LOG_WARNING);
	Log_Message(LOG_INFO, " ");
	Log_Message(LOG_INFO, "  " ENGINE);
	Log_Message(LOG_INFO, "  Version: " VERSION);
	Log_Message(LOG_INFO, " ");

	// Initialisation
	Log_Message(LOG_INFO, "------[ SETTING UP ]------");
	int succ = SDL_Init(
		SDL_INIT_VIDEO
		| SDL_INIT_EVENTS
		| SDL_INIT_AUDIO
	);
	if (succ < 0) {
		Log_SDLMessage(LOG_FATAL, "Failed to initialise SDL");
		return 1;
	}

	// TODO: Change Window Title
	Screen_Init(ENGINE " v" VERSION, SCREEN_WIDTH, SCREEN_HEIGHT);
	Events_Init(10);
	UserData_Init(); TTFText_Init();
	Sound_Init(); Pix_Init();

	Log_Message(LOG_INFO, "------[ GAME START ]------");

	// Load Scenes
	Scene_Register(scn_title, "title");
	Scene_Register(scn_dialogue, "dia");
	Scene_Register(scn_soundtest, "snd");
	Scene_Set("title");

	// Main Game Loop
	g_isRunning = true;
	Scene_Execute();


	// Termination
	Log_Message(LOG_INFO, "");
	Log_Message(LOG_INFO, "------[ TIDYING UP ]------");
	Pix_Term();
	Sound_Term();
	TTFText_Term(); UserData_Term();
	Screen_Term();

	Log_Message(LOG_INFO, "All done; Goodbye :)");
	SDL_Quit();
	return 0;
}
