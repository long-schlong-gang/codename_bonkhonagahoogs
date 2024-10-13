#ifndef DIALOGUE_H
#define DIALOGUE_H

//	
//	Handles visual-novel side of the engine
//	Displaying character portraits and loading/displaying
//	Interactive dialogue
//	

#include <SDL2/SDL.h>
#include <screen.h>
#include <log.h>

#include "colours.h"
#include "sound.h"


////	Constants
#define DIALOGUE_FILENAME "assets/txt/test_dialogue.dbf"


////	Types

// TODO: Dialogues, Nodes, Characters, etc.

////	Public Functions

//	Initialises the TTF Font System
//	
void TTFText_Init();


#endif