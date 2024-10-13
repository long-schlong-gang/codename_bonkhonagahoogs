#ifndef TTF_TEXT_H
#define TTF_TEXT_H

//	
//	Handles Rendering of text and text-boxes
//	using TTF Fonts and SDL_TTF
//	

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <screen.h>
#include <log.h>


////	Constants
#define TTFTEXT_FONT_FILE "assets/generale.otf"
#define TTFTEXT_FONT_SIZE 34
#define TTFTEXT_DEF_CLR 0xFF, 0xFF, 0xFF, 0xFF
#define TTFTEXT_NUM_CLRS 128

#define TTFTEXT_BOX_PADDING 10
#define TTFTEXT_BOX_BORDER_WIDTH 5
#define TTFTEXT_BOX_BORDER_CLR 0xFF, 0xFF, 0xFF, 0xFF
#define TTFTEXT_BOX_CLR 0x00, 0x00, 0x00, 0xFF


////	Types
typedef int TextColour;
#define TXTCLR_SPECIAL (TextColour)( -1 )
#define TXTCLR_RESET (TextColour)( 0 )


////	Global Vars
extern SDL_Colour g_TextColours[TTFTEXT_NUM_CLRS];


//	Initialises the TTF Font System
//	
void TTFText_Init();

//	Terminates the TTF Font System
//	
void TTFText_Term();

//	Gets the SDL_Colour for a TextColour
//	
SDL_Colour TTFText_GetColour(TextColour clr);

//	Renders a single Unicode glyph to the screen
//	
//	Glyphs in printable ASCII are cached
//	Returns  0 if successful,
//	Returns -1 otherwise; Nothing is displayed; Warning is logged
//	
//	NOTE: SDL_ttf Seems to have trouble with higher Unicode codepoints,
//	so it is not recommended to go about about U+8000, if you can help it
int TTFText_RenderGlyph(int x, int y, TextColour clr, Uint32 codepoint);

//	Renders a string at the given coordinates
//	
//	Takes a UTF-8 string
//	TODO: Custom escapes?
void TTFText_RenderText(int x, int y, TextColour clr, char *str);


// TODO:
//	Renders a text box with the given size and handles wrapping
//	
void TTFText_Textbox(int x, int y, int cols, int rows, TextColour clr, char *str);


#endif