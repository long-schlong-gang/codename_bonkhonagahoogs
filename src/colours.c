#include "colours.h"


SDL_Colour g_ColourPalette[CLR_COUNT] = {
	{ 0x40, 0x80, 0xFF, 0xFF },	// 0x0001: Window Background
	{ 0x00, 0x00, 0x00, 0xFF },	// 0x0002: Text-Box Background
	{ 0xFF, 0xFF, 0xFF, 0xFF },	// 0x0003: Text-Box Outline/Border
};


//void Colours_Init();

//void Colours_Term();

SDL_Colour Colours_GetRGBA(PaletteColour clr) {
	// Negative values are the special rainbow colour
	// TODO: More special colours? (Simple flashing)
	// TODO: Better Hue-circle style rainbow
	if (clr < 0) {
		Uint64 tick = SDL_GetTicks64();
		Uint8 r = (tick >> 4) + 0x00;
		Uint8 g = (tick >> 3) + 0x55;
		Uint8 b = (tick >> 2) + 0xAA;

		return (SDL_Colour){
			(r > 127) ? (r<<1) : 0xFF - (r<<1),
			(g > 127) ? (g<<1) : 0xFF - (g<<1),
			(b > 127) ? (b<<1) : 0xFF - (b<<1),
			0xFF
		};
	}

	// Colours out of bounds or zero return default colour (white)
	if (clr == 0 || clr > CLR_COUNT) {
		return (SDL_Colour){ 0xFF, 0xFF, 0xFF, 0xFF };
	}

	return g_ColourPalette[clr - 1];
}

void Colours_SetRGBA(PaletteColour clr, SDL_Colour rgba) {
	if (clr < 1 || clr > CLR_COUNT) {
		char msg[256];
		SDL_snprintf(msg, 256, "Tried to set invalid colour (0x%04X) in global palette", clr);
		Log_Message(LOG_WARNING, msg);
		return;
	}

	g_ColourPalette[clr - 1] = rgba;

	// Write change to userdata
	UserData_Set(UDATA_DBID_COLOURS, 0, &g_ColourPalette, sizeof(g_ColourPalette));
}

void Colours_SetRenderer(PaletteColour clr) {
	SDL_Colour rgba = Colours_GetRGBA(clr);
	SDL_SetRenderDrawColor(g_renderer, rgba.r, rgba.g, rgba.b, rgba.a);
}