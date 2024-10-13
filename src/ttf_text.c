#include "ttf_text.h"


SDL_Colour g_TextColours[TTFTEXT_NUM_CLRS];

static TTF_Font *__font = NULL;
static SDL_Texture *__glyph_cache[95]; // Cache of just printable ASCII glyphs
static SDL_Rect __dst_rect = { 0, 0, 0, 0 };


void TTFText_Init() {
	TTF_Init();

	__font = TTF_OpenFont(TTFTEXT_FONT_FILE, TTFTEXT_FONT_SIZE);
	if (__font == NULL) {
		char msg[512];
		SDL_snprintf(msg, 512, "Failed to open font '" TTFTEXT_FONT_FILE "', %ipt: %s", TTFTEXT_FONT_SIZE, TTF_GetError());
		Log_Message(LOG_ERROR, msg);
	}
}

void TTFText_Term() {
	if (__font == NULL) return;
	TTF_CloseFont(__font);

	TTF_Quit();
}

SDL_Colour TTFText_GetColour(TextColour clr) {
	
	// Negative values are the special rainbow colour
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
	if (clr == 0 || clr > TTFTEXT_NUM_CLRS) {
		return (SDL_Colour){ TTFTEXT_DEF_CLR };
	}

	return g_TextColours[clr - 1];
}

int TTFText_RenderGlyph(int x, int y, TextColour clr, Uint32 codepoint) {
	if (__font == NULL) return -1;

	// Check cache
	bool is_ascii = (codepoint > 32) && (codepoint < 127);
	int cache_index = codepoint - 32;
	SDL_Texture *tex = __glyph_cache[cache_index];

	// Render Glyph
	if (!is_ascii || tex == NULL) {
		SDL_Surface *surf = TTF_RenderGlyph32_Blended(__font, codepoint, (SDL_Colour){ 0xFF, 0xFF, 0xFF, 0xFF });
		if (surf == NULL) {
			char msg[512];
			char *fmt = "Failed to render glyph (U+%04X) to surface: %s";
			if (codepoint > UINT16_MAX) fmt = "Failed to render glyph (U+%08X) to surface: %s";
			SDL_snprintf(msg, 512, fmt, codepoint, TTF_GetError());
			Log_Message(LOG_WARNING, msg);
			return -1;
		}

		if (__dst_rect.w < 1) {
			__dst_rect.w = surf->w;
			__dst_rect.h = surf->h;
		}

		tex = SDL_CreateTextureFromSurface(g_renderer, surf);
		SDL_FreeSurface(surf);
		if (tex == NULL) {
			char msg[128];
			char *fmt = "Failed to render glyph (U+%04X) to texture";
			if (codepoint > UINT16_MAX) fmt = "Failed to render glyph (U+%08X) to texture";
			SDL_snprintf(msg, 128, fmt, codepoint);
			Log_SDLMessage(LOG_ERROR, msg);
			return -1;
		}

		if (is_ascii) __glyph_cache[cache_index] = tex;
	}

	// Draw to screen
	SDL_Colour rgb = TTFText_GetColour(clr);
	SDL_SetTextureColorMod(tex, rgb.r, rgb.g, rgb.b);
	__dst_rect.x = x; __dst_rect.y = y;
	SDL_RenderCopy(g_renderer, tex, NULL, &__dst_rect);
	if (!is_ascii) SDL_DestroyTexture(tex);

	return 0;
}

// TODO: Make more robust
static Uint32 __UTF8_NextCodepoint(char *str, char **next) {
	if (next == NULL) next = &str;

	Uint8 byte = **next; (*next)++;
	Uint32 codepoint = byte;

	// UTF-8 Conversion
	if (byte & 0x7F) {
		if (byte >> 5 == 0b110) {
			codepoint = (byte & 0b00011111) << 6;

			byte = **next; (*next)++; if (byte == 0) return 0;
			codepoint |= (byte & 0x3F);
		} else if (byte >> 4 == 0b1110) {
			codepoint = (byte & 0b00001111) << 6;

			byte = **next; (*next)++; if (byte == 0) return 0;
			codepoint |= (byte & 0x3F); codepoint <<= 6;
			byte = **next; (*next)++; if (byte == 0) return 0;
			codepoint |= (byte & 0x3F);
		} else if (byte >> 3 == 0b11110) {
			codepoint = (byte & 0b00000111) << 6;

			byte = **next; (*next)++; if (byte == 0) return 0;
			codepoint |= (byte & 0x3F); codepoint <<= 6;
			byte = **next; (*next)++; if (byte == 0) return 0;
			codepoint |= (byte & 0x3F); codepoint <<= 6;
			byte = **next; (*next)++; if (byte == 0) return 0;
			codepoint |= (byte & 0x3F);
		}
	}

	return codepoint;
}

void TTFText_RenderText(int x, int y, TextColour clr, char *str) {
	if (__font == NULL) {
		Log_Message(LOG_ERROR, "Tried Rendering text before initialising TTFText System");
		return;
	}

	int nx = x;
	char *next = str;
	while (*next != '\0') {
		Uint32 codepoint = __UTF8_NextCodepoint(str, &next);
		if (codepoint == 0) break;

		int succ = TTFText_RenderGlyph(nx, y, clr, codepoint);
		if (succ == 0) nx += __dst_rect.w;
	}

}

void TTFText_Textbox(int x, int y, int cols, int rows, TextColour clr, char *str) {
	
	// Draw Box
	SDL_SetRenderDrawColor(g_renderer, TTFTEXT_BOX_CLR);
	SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
		x + TTFTEXT_BOX_BORDER_WIDTH,
		y + TTFTEXT_BOX_BORDER_WIDTH,
		cols*__dst_rect.w + 2*TTFTEXT_BOX_PADDING,
		rows*__dst_rect.h + 2*TTFTEXT_BOX_PADDING,
	});

	SDL_SetRenderDrawColor(g_renderer, TTFTEXT_BOX_BORDER_CLR);
	SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
		x,
		y,
		cols*__dst_rect.w + 2*TTFTEXT_BOX_PADDING + 2*TTFTEXT_BOX_BORDER_WIDTH,
		TTFTEXT_BOX_BORDER_WIDTH
	});
	SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
		x,
		y,
		TTFTEXT_BOX_BORDER_WIDTH,
		rows*__dst_rect.h + 2*TTFTEXT_BOX_PADDING + 2*TTFTEXT_BOX_BORDER_WIDTH,
	});
	SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
		x + cols*__dst_rect.w + 2*TTFTEXT_BOX_PADDING + TTFTEXT_BOX_BORDER_WIDTH,
		y,
		TTFTEXT_BOX_BORDER_WIDTH,
		rows*__dst_rect.h + 2*TTFTEXT_BOX_PADDING + 2*TTFTEXT_BOX_BORDER_WIDTH,
	});
	SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
		x,
		y + rows*__dst_rect.h + 2*TTFTEXT_BOX_PADDING + TTFTEXT_BOX_BORDER_WIDTH,
		cols*__dst_rect.w + 2*TTFTEXT_BOX_PADDING + 2*TTFTEXT_BOX_BORDER_WIDTH,
		TTFTEXT_BOX_BORDER_WIDTH,
	});

	// Draw Text
	char *next = str;
	for (int yb=0; yb<rows; yb++) {
		for (int xb=0; xb<cols; xb++) {
			Uint32 codepoint = __UTF8_NextCodepoint(str, &next);
			if (codepoint == 0) return;

			if (codepoint == '\n') {
				xb = 0;
				yb++;
				continue;
			}

			TTFText_RenderGlyph(
				x + xb*__dst_rect.w + TTFTEXT_BOX_BORDER_WIDTH + TTFTEXT_BOX_PADDING,
				y + yb*__dst_rect.h + TTFTEXT_BOX_BORDER_WIDTH + TTFTEXT_BOX_PADDING,
				clr, codepoint
			);
		}
	}

}