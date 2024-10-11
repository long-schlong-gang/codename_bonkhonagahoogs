//	
//	Test Server using my Gin-Tonic Library's Networking functions
//	

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#include <log.h>
#include <util.h>
#include <screen.h>
#include <sprite.h>
#include <text.h>
#include <events.h>


int main(int argc, char* args[]) {
	// Set Logging to Develop Mode
	Log_SetPrintLevel(LOG_DEBUG);
	Log_SetPopupLevel(LOG_WARNING);


	// Initialisation
	if (SDL_Init(SDL_INIT_VIDEO) < 0) Log_SDLMessage(LOG_FATAL, "Failed to initialise SDL");
	Log_Message(LOG_DEBUG, "Hello, world!");
	Log_Message(LOG_INFO, "Hello, world!");

	Screen_Init("GinTonic Test Server", 256, 256);
	Sprite_Init();
	Text_Init(NULL);
	Events_Init(50);
	

	// Main Loop
	bool isRunning = true;
	bool redraw = true;
	SDL_Event curr_event;
	while (isRunning) {

		// Handle events
		int scode = SDL_WaitEvent(&curr_event);
		if (scode != 0) {
			Events_HandleInternal(curr_event);

			switch (curr_event.type) {
				case SDL_QUIT:
					isRunning = false;
				continue;
			}
		}

		if (redraw) {
			SDL_SetRenderDrawColor(g_renderer, 0x10, 0x40, 0x80, 0xFF);
			SDL_RenderClear(g_renderer);

			Text_Draw("Hello, world!", 37, 10, 20);
			
			SDL_RenderPresent(g_renderer);
		}

	}


	// Termination
	Text_Term();
	Sprite_Term();
	Screen_Term();
	SDL_Quit();
	return 0;
}
