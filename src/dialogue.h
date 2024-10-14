#ifndef DIALOGUE_H
#define DIALOGUE_H

//	
//	Handles visual-novel side of the engine
//	Displaying character portraits and loading/displaying
//	Interactive dialogue
//	
//	Nice-To-Have:
//	 - Rewrite with custom arena allocator for dialogue nodes & strings

#include <SDL2/SDL.h>
#include <datablock.h>
#include <binding.h>
#include <screen.h>
#include <log.h>

#include "ttf_text.h"
#include "colours.h"
#include "sound.h"


////	Constants
#define DIALOGUE_FILENAME "assets/txt/test_dialogue.dbf"
#define DIA_FILE_HEADER_DBID 0x0000
#define DIA_FILE_HEADER_SIZE 2

#define DIA_BOX_POS_X	50
#define DIA_BOX_POS_Y	600
#define DIA_BOX_COLS	35
#define DIA_BOX_ROWS	8
#define DIA_BOX_SIZE	(DIA_BOX_COLS*DIA_BOX_ROWS)

#define DIA_TEXT_ESC 0x1B
#define DIA_FILE_PAGE_SEP 0x1D


////	Types

typedef Uint16 NodeID;
#define NULL_NODE (NodeID)(0x0000)

typedef struct {
	char *text;
	NodeID next;
} Dialogue_Response;

typedef struct {
	char *text;
	int state; // 0: Don't show; >0: typing; <0: Finished
} Dialogue_Node;

typedef struct {
	Dialogue_Node *root;
	Dialogue_Node *current;
	int state;
} Dialogue_Tree;


////	Globals
extern Dialogue_Tree g_CurrentDialogue;


////	Public Functions

//	Load dialogue tree from a file
//	
//	Sets `g_CurrentDialogue`; clears previous dialogue, if set
void Dialogue_LoadTree(char *filename);

//	Unloads the current dialogue tree
//	
void Dialogue_UnloadTree();

//	Handles dialogue-system events
//	
//	Should be called for every SDL event received
void Dialogue_HandleEvents(SDL_Event event);

//	Handles drawing dialogue boxes
//	
//	Should be called for every draw call
void Dialogue_DrawAll();


//	Loads a single node from a datablock file
//	
//	Returns a pointer or NULL on error
//	Logs appropriate errors/warnings
Dialogue_Node *Dialogue_LoadNode(Datablock_File *dbf, NodeID dbid);

//	Frees a Dialogue_Node
//	
//	Must be called when a node is no longer needed
void Dialogue_FreeNode(Dialogue_Node *node);

//	Draws a single node
//	
//	Used internally, mainly
void Dialogue_DrawNode(Dialogue_Node *node);


#endif