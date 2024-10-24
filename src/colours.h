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
#define CLR_COUNT 0x10


////	Types
typedef Sint16 PaletteColour;

#define CLR_SPECIAL		(PaletteColour)( -1 )
#define CLR_BLACK		(PaletteColour)( 0x00 )
#define CLR_WINDOW_BG	(PaletteColour)( 0x01 )
#define CLR_TXTBOX_BG	(PaletteColour)( 0x02 )
#define CLR_TXTBOX_BRDR	(PaletteColour)( 0x03 )
#define CLR_TEXT_NORM	(PaletteColour)( 0x04 )
#define CLR_TEXT_EMPH	(PaletteColour)( 0x05 )
#define CLR_BTN_NORMAL	(PaletteColour)( 0x06 )
#define CLR_BTN_SELECT	(PaletteColour)( 0x07 )
#define CLR_BTN_BLOCKED	(PaletteColour)( 0x08 )
#define CLR_PLANET_TUN	(PaletteColour)( 0x09 )
#define CLR_PLANET_VES	(PaletteColour)( 0x0A )
#define CLR_CORPORATE	(PaletteColour)( 0x0B )
#define CLR_TILE_ITABLE	(PaletteColour)( 0x0C )
#define CLR_NAME_LEV	(PaletteColour)( 0x0D )
#define CLR_NAME_ERU	(PaletteColour)( 0x0E )
#define CLR_NAME_FED	(PaletteColour)( 0x0F )
#define CLR_NAME_KELEN	(PaletteColour)( 0x10 )


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
//	If `clr` is out of bounds, it just returns black
SDL_Colour Colours_GetRGBA(PaletteColour clr);

//	Sets a global colour palette colour
//	
//	(Writes to userdata)
void Colours_SetRGBA(PaletteColour clr, SDL_Colour rgba);

//	Sets the global renderer's draw colour
//	
void Colours_SetRenderer(PaletteColour clr);

#endif