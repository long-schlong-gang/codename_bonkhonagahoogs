#ifndef TTF_TEXT_H
#define TTF_TEXT_H

//	
//	Handles Rendering of text and text-boxes
//	using TTF Fonts and SDL_TTF
//	
//	TODO:	Maybe cache full rendered texture for repeated draws
//			Alternate versions of text rendering functions that return an `SDL_Texture`

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <screen.h>
#include <log.h>

#include "colours.h"
#include "gamestate.h"


////	Constants
#define TTFTEXT_FONT_FILE "assets/fonts/generale.otf"
#define TTFTEXT_FONT_SIZE 34

// Depends on TTFTEXT_FONT_SIZE; if you change it, re-calc glyph size
#define TTFTEXT_GLYPH_W 25
#define TTFTEXT_GLYPH_H 41

#define TTFTEXT_BOX_PADDING 10
#define TTFTEXT_BOX_BORDER_WIDTH 5

#define TTFTEXT_ESC 0x1B


////	Types
typedef struct {
	int x; int y;
	int cols; int rows;
	PaletteColour clr;
	int charcount;
	char *str;
	//TODO: SDL_Texture *full_render; // Saves time for repeat draws
} TTFText_Box;


////	Public Functions

//	Initialises the TTF Font System
//	
void TTFText_Init();

//	Terminates the TTF Font System
//	
void TTFText_Term();

//	Gets the width of rendered characters
//	
int TTFText_GlyphWidth();

//	Gets the height of rendered characters
//	
int TTFText_GlyphHeight();

//	Renders a single Unicode glyph to the screen
//	
//	Glyphs in printable ASCII are cached
//	Returns  0 if successful,
//	Returns -1 otherwise; Nothing is displayed; Warning is logged
//	
//	NOTE: SDL_ttf Seems to have trouble with higher Unicode codepoints,
//	so it is not recommended to go about about U+8000, if you can help it
int TTFText_RenderGlyph(int x, int y, PaletteColour clr, Uint32 codepoint);

//	Renders a string at the given coordinates
//	
//	Takes a UTF-8 string
//	TODO: Provide optional rect pointer; If w/h set, limit text to given size, else (0/0), set w/h to final size
//	TODO: Custom escapes? (General escape handler?)
void TTFText_RenderText(int x, int y, PaletteColour clr, char *str);

//	Renders a text box with the given size and handles wrapping
//	
//	Inserts line breaks after `cols` characters on a row,
//	Inserts line breaks for '\n' chars
//	The last parameter `charcount` determines how many chars are drawn
//	this lets you have the text slowly type out over multiple calls
//	If it's negative, the whole text is drawn all at once
//	
//	Returns how many characters were drawn; -1 once it's rendered them all
//	TODO: Custom escapes!
int TTFText_Draw_Box(TTFText_Box txt);

#endif