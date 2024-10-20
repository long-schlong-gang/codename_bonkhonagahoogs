#include "pix.h"


// TODO: Embed somehow?

ImageInfo g_Images[PIX_COUNT] = {
	{ "assets/img/image.jpg", 100, 100, NULL },
	{ "assets/img/levu.jpg", 378, 1155, NULL },
	{ "assets/img/levu_happy.jpg", 378, 1155, NULL },
	{ "assets/img/levu_angry.jpg", 378, 1155, NULL },
	{ "assets/img/eruya.jpg", 339, 859, NULL },
	{ "assets/img/eruya_happy.jpg", 339, 859, NULL },
	{ "assets/img/eruya_angry.jpg", 339, 859, NULL },
	{ "assets/img/fedelov.jpg", 371, 870, NULL },
	{ "assets/img/fedelov_happy.jpg", 371, 870, NULL },
	{ "assets/img/fedelov_angry.jpg", 371, 870, NULL },
	{ "assets/img/player_up.jpg", 100, 100, NULL },
	{ "assets/img/player_left.jpg", 100, 100, NULL },
	{ "assets/img/player_right.jpg", 100, 100, NULL },
	{ "assets/img/player_down.jpg", 100, 100, NULL },
};


void Pix_Init() {
	int succ = IMG_Init(PIX_INIT_FLAGS);
	if (succ == 0) {
		char msg[512];
		SDL_snprintf(msg, 512, "Failed to initialise the image system: %s", IMG_GetError());
		Log_Message(LOG_FATAL, msg);
	}

	Log_Message(LOG_INFO, "Successfully Initialised Image System!");
}

void Pix_Term() {
	for (int i=0; i<PIX_COUNT; i++) {
		if (g_Images[i].tex != NULL) SDL_DestroyTexture(g_Images[i].tex);
	}
	IMG_Quit();

	Log_Message(LOG_INFO, "Terminated Image System");
}

void Pix_Load(Image pic) {
	if (pic < 0 || pic >= PIX_COUNT) return;
	if (g_Images[pic].tex != NULL) return;

	g_Images[pic].tex = IMG_LoadTexture(g_renderer, g_Images[pic].filename);
	if (g_Images[pic].tex == NULL) {
		char msg[512];
		SDL_snprintf(msg, 512, "Failed to load image No. %i (%s): %s", pic, g_Images[pic].filename, IMG_GetError());
		Log_Message(LOG_ERROR, msg);
	}
}

void Pix_Draw(Image pic, int x, int y, int w, int h) {
	if (pic < 0 || pic >= PIX_COUNT) return;

	if (g_Images[pic].tex == NULL) Pix_Load(pic);

	SDL_Rect dst = {
		x, y, w, h,
	};
	if (w < 0) dst.w = g_Images[pic].w;
	if (h < 0) dst.h = g_Images[pic].h;

	SDL_RenderCopy(g_renderer, g_Images[pic].tex, NULL, &dst);
}

void Pix_Clear(Image pic) {
	if (pic < 0 || pic >= PIX_COUNT) return;
	if (g_Images[pic].tex == NULL) return;

	SDL_DestroyTexture(g_Images[pic].tex);
	g_Images[pic].tex = NULL;
}