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

#define PIX_INIT_FLAGS (IMG_INIT_JPG | IMG_INIT_PNG)

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
	PIX_PLAYER_DOWN_F2,
	PIX_TITLE_SPLASH,
	PIX_ROOM_QUART_CAPTAIN,
	PIX_ROOM_BRIDGE,
	PIX_ROOM_HALLWAY,
	PIX_ROOM_QUARTERS_NORTH,
	PIX_ROOM_QUARTERS_SOUTH,
	PIX_ROOM_CAFETERIA,
	PIX_ROOM_CARGO,
	PIX_INTERFACE_NORMAL,
	PIX_INTERFACE_WARNING,
} Image;
#define PIX_COUNT 25


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