#include "colours.h"


SDL_Colour g_ColourPalette[CLR_COUNT] = {
	{ 0x00, 0x00, 0x00, 0xFF },	// 0x0001: Window Background
	{ 0x08, 0x08, 0x08, 0xFF },	// 0x0002: Text-Box Background
	{ 0xFF, 0xFF, 0xFF, 0xFF },	// 0x0003: Text-Box Outline/Border
	{ 0xC0, 0xC0, 0xC0, 0xFF },	// 0x0004: Regular Text
	{ 0xFF, 0xFF, 0xC0, 0xFF },	// 0x0005: Emphasised Text
	{ 0xFF, 0xFF, 0xFF, 0xFF },	// 0x0006: Regular Text-Button
	{ 0x20, 0xE0, 0x40, 0xFF },	// 0x0007: Highlighted/Selected Text-Button
	{ 0x80, 0x80, 0x80, 0xFF },	// 0x0008: Disabled Text-Button
	{ 0x40, 0x80, 0xFF, 0xFF },	// 0x0009: Planet Tunatese
	{ 0xF0, 0x80, 0x20, 0xFF },	// 0x000A: Planet Veseto
	{ 0xFF, 0xD0, 0x40, 0xFF },	// 0x000B: Colour of the company "Neravess"
	{ 0x20, 0xE0, 0x40, 0x20 },	// 0x000C: Highlighted interactable Tiles
	{ 0xC0, 0xC0, 0xB0, 0xFF },	// 0x000D: Levu's Name
	{ 0xEB, 0x40, 0x34, 0xFF },	// 0x000E: Eruya's Name
	{ 0x50, 0x8A, 0xE6, 0xFF },	// 0x000F: Fedelov's Name
	{ 0x82, 0x41, 0x19, 0xFF },	// 0x0010: Sme~~Ke~~len's Name
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

	// Colour out of bounds or zero; return black
	if (clr == 0 || clr > CLR_COUNT) {
		return (SDL_Colour){ 0x00, 0x00, 0x00, 0xFF };
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