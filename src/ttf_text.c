#include "ttf_text.h"


static TTF_Font *__font = NULL;
static SDL_Texture *__glyph_cache[95]; // Cache of just printable ASCII glyphs

#ifndef TTFTEXT_GLYPH_W
static SDL_Rect __dst_rect = { 0, 0, 0, 0 };
#else
static SDL_Rect __dst_rect = { 0, 0, TTFTEXT_GLYPH_W, TTFTEXT_GLYPH_H };
#endif


void TTFText_Init() {
	TTF_Init();

	__font = TTF_OpenFont(TTFTEXT_FONT_FILE, TTFTEXT_FONT_SIZE);
	if (__font == NULL) {
		char msg[512];
		SDL_snprintf(msg, 512, "Failed to open font '" TTFTEXT_FONT_FILE "', %ipt: %s", TTFTEXT_FONT_SIZE, TTF_GetError());
		Log_Message(LOG_ERROR, msg);
	}

	Log_Message(LOG_INFO, "Successfully Initialised Text System!");
}

void TTFText_Term() {
	if (__font == NULL) return;
	TTF_CloseFont(__font);
	TTF_Quit();

	Log_Message(LOG_INFO, "Terminated Text System");
}

int TTFText_GlyphWidth() {
	#ifdef TTFTEXT_GLYPH_W
	return TTFTEXT_GLYPH_W;
	#else
	return (__dst_rect.w > 0) ? __dst_rect.w : TTFTEXT_FONT_SIZE;
	#endif
}

int TTFText_GlyphHeight() {
	#ifdef TTFTEXT_GLYPH_H
	return TTFTEXT_GLYPH_H;
	#else
	return (__dst_rect.h > 0) ? __dst_rect.h : TTFTEXT_FONT_SIZE;
	#endif
}


int TTFText_RenderGlyph(int x, int y, PaletteColour clr, Uint32 codepoint) {
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

		#ifndef TTFTEXT_GLYPH_W
		if (__dst_rect.w < 1) {
			__dst_rect.w = surf->w;
			__dst_rect.h = surf->h;

			// DEBUG:
			char msg[256];
			SDL_snprintf(msg, 256, "Rendered Glyph Size: %i x %i px", surf->w, surf->h);
			Log_Message(LOG_DEBUG, msg);
		}
		#endif

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
	SDL_Colour rgb = Colours_GetRGBA(clr);
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

void TTFText_RenderText(int x, int y, PaletteColour clr, char *str) {
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

int TTFText_Draw_Box(TTFText_Box txt) {
	
	// Draw Box
	Colours_SetRenderer(CLR_TXTBOX_BG);
	SDL_RenderFillRect(g_renderer, &(struct SDL_Rect){
		txt.x + TTFTEXT_BOX_BORDER_WIDTH,
		txt.y + TTFTEXT_BOX_BORDER_WIDTH,
		txt.cols*__dst_rect.w + 2*TTFTEXT_BOX_PADDING,
		txt.rows*__dst_rect.h + 2*TTFTEXT_BOX_PADDING,
	});

	Colours_SetRenderer(CLR_TXTBOX_BRDR);
	SDL_RenderFillRects(g_renderer, (struct SDL_Rect[4]){
		{
			txt.x,
			txt.y,
			txt.cols*__dst_rect.w + 2*TTFTEXT_BOX_PADDING + 2*TTFTEXT_BOX_BORDER_WIDTH,
			TTFTEXT_BOX_BORDER_WIDTH
		}, {
			txt.x,
			txt.y,
			TTFTEXT_BOX_BORDER_WIDTH,
			txt.rows*__dst_rect.h + 2*TTFTEXT_BOX_PADDING + 2*TTFTEXT_BOX_BORDER_WIDTH,
		}, {
			txt.x + txt.cols*__dst_rect.w + 2*TTFTEXT_BOX_PADDING + TTFTEXT_BOX_BORDER_WIDTH,
			txt.y,
			TTFTEXT_BOX_BORDER_WIDTH,
			txt.rows*__dst_rect.h + 2*TTFTEXT_BOX_PADDING + 2*TTFTEXT_BOX_BORDER_WIDTH,
		}, {
			txt.x,
			txt.y + txt.rows*__dst_rect.h + 2*TTFTEXT_BOX_PADDING + TTFTEXT_BOX_BORDER_WIDTH,
			txt.cols*__dst_rect.w + 2*TTFTEXT_BOX_PADDING + 2*TTFTEXT_BOX_BORDER_WIDTH,
			TTFTEXT_BOX_BORDER_WIDTH,
		}
	}, 4);

	// Just draw the box
	if (txt.charcount == 0) return 0;

	// Draw Text
	PaletteColour curr_clr = txt.clr;
	int count = 0;
	char *next = txt.str;
	for (int yb=0; yb<txt.rows; yb++) {
		for (int xb=0; xb<txt.cols; xb++) {
			Uint32 codepoint = __UTF8_NextCodepoint(txt.str, &next);
			if (codepoint == 0) return -1;

			while (codepoint == TTFTEXT_ESC) {
				Uint8 code = *next; next++;
				switch (code) {
					case 0x00: curr_clr = CLR_TEXT_NORM; break;
					case 0x01: curr_clr = CLR_TEXT_EMPH; break;
					case 0x02: {
						PaletteColour clr = *next; next++;
						clr <<= 8;
						clr |= *next; next++;
						curr_clr = clr;
					} break;
					case 0x0F: curr_clr = CLR_SPECIAL; break;
					
					// Set global flags
					case 0xE0: {
						static Uint8 last_key = 0;
						static Uint8 last_val = 0;
						Uint8 key = *next; next++;
						Uint8 val = *next; next++;

						if (key == last_key && val == last_val) break;
						last_key = key; last_val = val;
						Gamestate_SetFlag(key, val);
					} break;
				}

				codepoint = __UTF8_NextCodepoint(txt.str, &next);
				if (codepoint == 0) return -1;
			}

			if (codepoint == '\n') {
				xb = -1;
				yb++;
				continue;
			} 

			TTFText_RenderGlyph(
				txt.x + xb*__dst_rect.w + TTFTEXT_BOX_BORDER_WIDTH + TTFTEXT_BOX_PADDING,
				txt.y + yb*__dst_rect.h + TTFTEXT_BOX_BORDER_WIDTH + TTFTEXT_BOX_PADDING,
				curr_clr, codepoint
			);
			count++;

			if (txt.charcount > 0 && count >= txt.charcount) return count;
		}
	}

	return -1;
}