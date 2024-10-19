#ifndef MENU_ELEMENT_H
#define MENU_ELEMENT_H

//		
//		Interactive Element System
//		
//		Handles things like buttons
//		
//		To-Do:
//		 - Add optional sound effects to buttons (should be handled by callback?)


#include <SDL2/SDL.h>
#include "ttf_text.h"


////	Constants

#define MENEL_MAX_ELEMENTS 256

#define MENEL_TXTBTN_PADDING 2
#define MENEL_TXTBTN_OUTLINE 2


////	Types

typedef enum {
	MENEL_BTN_NORMAL,
	MENEL_BTN_DISABLED,
	MENEL_BTN_HIGHLIGHTED,
	//MENEL_BTN_SELECTED,
} Menel_Btn_State;

//	TODO: Make generic element with union?
//typedef struct {
//	void (*draw_element)(void *mel);
//	bool (*handle_input)(void *mel, SDL_Event *event);
//	Menel_Element_State state;
//	SDL_Rect bounding_box;
//	void *data;
//} Menel_Element;

//	Callback triggered when an interactive element moves into a new state
//	
typedef void (*Menel_Callback)(void *udata);

typedef struct {
	//void (*draw_element)(void *btn);
	//bool (*handle_input)(void *btn, SDL_Event *event);
	Menel_Btn_State state;
	SDL_Rect bounding_box;
	Menel_Callback on_highlight;
	Menel_Callback on_select;
	void *user_data;
	char *text;
} Menel_TextButton;

typedef struct {
	Menel_TextButton *btns;
	int btn_count;
	int sel_index;
} Menel_TextButtonArray;

////	Global Variables

////	Public Functions

//	Initialises the Menu Element System
//	
//void Menel_Init();
//
////	Terminates the Menu Element System
////	
//void Menel_Term();
//
////	Clears out all currently registered Menu Elements
////	
//void Menel_ClearAll();
//
////	Handles all SDL_Events relevant to the Menu Element system
////	
//void Menel_HandleEvents(SDL_Event event);
//
////	Draws all currently registered menu elements to the screen
////	
//void Menel_DrawElements();
//


//	Create a simple Text-Button
//	
//	Caller is responsible for text; it should either be a constant, or
//	remain allocated until the corresponding `Menel_TextBtn_Destroy()`.
Menel_TextButton *Menel_TextBtn_Create(SDL_Rect shape, char *text, Menel_Callback on_highlight, Menel_Callback on_select);

//	Frees a button
//	
void Menel_TextBtn_Destroy(Menel_TextButton *btn);

//	Handle input events for a text button
//	
void Menel_TextBtn_HandleEvent(Menel_TextButton *btn, SDL_Event event);

//	Draw a text button to the screen
//	
void Menel_TextBtn_Draw(Menel_TextButton *btn);


//	Create a simple Text-Button Array
//	
//	~~This function provides an extra callback `on_choose`,
//	~~that is called whenever any of the buttons is selected.
//	~~The index of the currently selected/highlighted button is
//	~~stored in the array struct `sel_index`. If this is negative,
//	~~then no option has been selected yet.
//	
//	At the moment, both left and up count as going "back" in the list,
//	while both right and down count as going "forwards".
Menel_TextButtonArray *Menel_TBtnArr_Create(int num, Menel_TextButton *buttons);

//	Frees a button
//	
void Menel_TBtnArr_Destroy(Menel_TextButtonArray *arr);

//	Sets the state of all the buttons in the array
//	
void Menel_TBtnArr_SetAllState(Menel_TextButtonArray *arr, Menel_Btn_State state);

//	Clears all buttons in the array of the "selected" or "highlighted" state
//	
//	(Leaves "disabled" buttons alone)
void Menel_TBtnArr_ClearSelection(Menel_TextButtonArray *arr);

//	Moves the selection by some amount through the list
//	
//	if move is negative, the selection moves that many buttons "backwards"
//	if move is positive, the selection moves that many buttons "forewards"
void Menel_TBtnArr_MoveSelectionWrap(Menel_TextButtonArray *arr, int move);

//	Handle input events for an array of text buttons
//	
void Menel_TBtnArr_HandleEvent(Menel_TextButtonArray *arr, SDL_Event event);

//	Draw an array of text buttons to the screen
//	
void Menel_TBtnArr_Draw(Menel_TextButtonArray *arr);


#endif