#ifndef MENU_ELEMENT_H
#define MENU_ELEMENT_H


#include <SDL2/SDL.h>
#include "ttf_text.h"


////	Constants

#define MENEL_MAX_ELEMENTS 256


////	Types

typedef enum {
	MENEL_NORMAL,
	MENEL_DISABLED,
	MENEL_HIGHLIGHTED,
	MENEL_SELECTED,
} Menu_Element_State;

typedef struct {
	void (*draw_element)(void *mel);
	bool (*handle_input)(void *mel, SDL_Event *event);
	Menu_Element_State state;
	SDL_Rect bounding_box;
	void *data;
} Menu_Element;

typedef void (*Menu_Callback)(Menu_Element *mel, void *data);

typedef struct {
	SDL_Texture *background;
} Menu_Button;


////	Global Variables


////	Public Functions

//	Initialises the Menu Element System
//	
void Menel_Init();

//	Terminates the Menu Element System
//	
void Menel_Term();

//	Clears out all currently registered Menu Elements
//	
void Menel_ClearAll();

//	Handles all SDL_Events relevant to the Menu Element system
//	
void Menel_HandleEvents(SDL_Event event);

//	Draws all currently registered menu elements to the screen
//	
void Menel_DrawElements();


//	Create a simple pushbutton with a background image
//	
void Menel_CreateButton(SDL_Rect shape, SDL_Texture *tex, Menu_Callback on_highlight, Menu_Callback on_select);


#endif