#ifndef PIX_H
#define PIX_H

//	
//	Handles image loading/displaying
//	

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <screen.h>
#include <log.h>


////	Constants

#define PIX_INIT_FLAGS (IMG_INIT_JPG)

////	Types

typedef struct {
	const char *filename;
	int w;
	int h;
	SDL_Texture *tex;
} ImageInfo;

typedef enum {
	PIX_TIT_SMILE,
	PIX_LEVU_NEUTRAL,
	PIX_LEVU_HAPPY,
	PIX_LEVU_ANGRY,
	PIX_ERUYA_NEUTRAL,
	PIX_ERUYA_HAPPY,
	PIX_ERUYA_ANGRY,
	PIX_FEDELOV_NEUTRAL,
	PIX_FEDELOV_HAPPY,
	PIX_FEDELOV_ANGRY,
	PIX_PLAYER_UP,
	PIX_PLAYER_LEFT,
	PIX_PLAYER_RIGHT,
	PIX_PLAYER_DOWN,
} Image;
#define PIX_COUNT 14


////	Globals

extern ImageInfo g_Images[PIX_COUNT];


////	Public Functions

//	Initialises the Images System
//	
void Pix_Init();

//	Terminates the Image System
//	
void Pix_Term();

//	Loads an image so it's ready to draw
//	
//	Should be done before calling Pix_Draw
void Pix_Load(Image pic);

//	Draws a picture to the screen
//	
//	if `w` or `h` are negative, the original image's
//	width/height will be used instead
//	
//	Will prepare the image automatically if it wasn't already done
void Pix_Draw(Image pic, int x, int y, int w, int h);

//	Frees the memory associated with a specific image
//	
//	Should be called when an image is no longer needed
void Pix_Clear(Image pic);


#endif