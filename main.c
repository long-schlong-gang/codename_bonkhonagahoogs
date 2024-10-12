//	
//	PROJECT BONKHONAGAHOOGS
//	
//	Source Code v0.1.0
//	By Olorin
//	

#define VERSION "0.1.0"

#include <SDL2/SDL.h>

#include <log.h>
#include <util.h>
#include <screen.h>
#include <sprite.h>
#include <events.h>
#include <menu.h>
#include "src/userdata.h"


//	Include Scenes
#include "scn_title.c"


int main(int argc, char* args[]) {
	// Set Logging to Develop Mode
	// TODO: Change for Prod
	Log_SetPrintLevel(LOG_DEBUG);
	Log_SetPopupLevel(LOG_WARNING);

	// Initialisation
	if (SDL_Init(SDL_INIT_VIDEO) < 0) Log_SDLMessage(LOG_FATAL, "Failed to initialise SDL");

	// TODO: Change Window Title
	Screen_Init("Proj. BHGH v" VERSION, 256, 256);
	Sprite_Init();
	Text_Init(NULL);
	Events_Init(50);
	UserData_Init();

	// DEBUG: Test userdata writing
	float vol = 0.0f;
	int C = 0;
	printf("--->[00] C: %i, Volume: %f\n", C, vol);

	C = UserData_Get(0x0002, 0, &vol, sizeof(vol));
	printf("--->[01] C: %i, Volume: %f\n", C, vol);

	vol = 0.3145f;
	C = UserData_Set(0x0002, 0, &vol, sizeof(vol));
	printf("--->[02] C: %i, Volume: %f\n", C, vol);

	C = UserData_Get(0x0002, 0, &vol, sizeof(vol));
	printf("--->[03] C: %i, Volume: %f\n", C, vol);

	// Load Scenes
	Scene_Register(scn_title, "title");
	Scene_Set("title");

	// Main Game Loop
	g_isRunning = true;
	Scene_Execute();

	// Termination
	UserData_Term();
	Text_Term();
	Sprite_Term();
	Screen_Term();
	SDL_Quit();
	return 0;
}
