#ifndef COLOURS_H
#define COLOURS_H

//	
//	Creates a single place where colours are managed globally
//	Allows later palette adjustments (maybe user themes, lol)
//	

#include <SDL2/SDL.h>
#include <screen.h>

#include "userdata.h"


////	Constants
#define CLR_COUNT 3


////	Types
typedef Sint16 PaletteColour;

#define CLR_SPECIAL 	(PaletteColour)( -1 )
#define CLR_RESET		(PaletteColour)( 0 )
#define CLR_WINDOW_BG	(PaletteColour)( 1 )
#define CLR_TXTBOX_BG	(PaletteColour)( 2 )
#define CLR_TXTBOX_BRDR	(PaletteColour)( 3 )


////	Global Vars
extern SDL_Colour g_ColourPalette[CLR_COUNT];


////	Public Functions

//	Initialises the global colour palette
//	
//	Loads palette from user-data (User data must be initted)
//void Colours_Init();

//	Terminates the global colour palette
//	
//void Colours_Term();

//	Gets colour from the global colour palette
//	
SDL_Colour Colours_GetRGBA(PaletteColour clr);

//	Sets a global colour palette colour
//	
//	(Writes to userdata)
void Colours_SetRGBA(PaletteColour clr, SDL_Colour rgba);

//	Sets the global renderer's draw colour
//	
void Colours_SetRenderer(PaletteColour clr);

#endif