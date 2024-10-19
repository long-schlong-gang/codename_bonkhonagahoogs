#ifndef USERDATA_H
#define USERDATA_H

//	
//	Handles Storage and retrieval
//	of user data stored locally.
//	
//	TODO:
//	 - Find out why it can't write sometimes and why it crashes

#include <stdio.h>
#include <datablock.h>
#include <binding.h>


#define UDATA_FILENAME	"userdata.dbf"	// Where to find userdata

//	Userdata File Datablock-IDs/Types
#define UDATA_DBID_BINDINGS	0x0001		// Keybinding Preferences
#define UDATA_DBID_AUDIOPRF	0x0002		// Audio Preferences
#define UDATA_DBID_COLOURS	0x0003		// Global Colour Palette
#define UDATA_DBID_SAVEINFO	0x0100		// User Save data header


////	Global Vars
extern Datablock_File *g_Userdata_File;


////	Public Function Declarations

//	Checks if the userdata file exists and opens it
//	
//	If it doesn't exist, it is created with default settings.
//	Crashes if it can't create the settings file.
//	Initialises Gin-Tonic Binding system.
void UserData_Init();

//	Terminates the userdata system
//	
//	Terminates Gin-Tonic Binding system.
void UserData_Term();

//	Fetches data from local user data
//	
//	Gets the data from the user data file in the
//	`num`th datablock of type `type`
//	
//	Returns 1 on successful read with valid checksum,
//	Returns 0 on successful read but with invalid checksum; logs a warning
//	Returns -1 if the block couldn't be found; logs a warning
//	Returns -2 if the userdata was larger than `size`, or `data` is NULL; logs an error
int UserData_Get(Uint16 type, int num, void *data, size_t size);

//	Updates data in local user data
//	
//	Sets the data from the user data file in the
//	`num`th datablock of type `type`
//	It cannot create any new blocks or extend existing ones,
//	it only overwrites existing blocks with the same amount of data!
//	If the provided buffer is smaller than the userdata space,
//	the remaining bytes will be unaffected in the file
//	
//	Returns 0 on successful write,
//	Returns -1 if the block couldn't be found; logs an error,
//	Returns -2 if `size` is larger than the userdata length, or `data` is NULL; logs an error
//	Returns -3 if the file can't be overwritten; logs an error
int UserData_Set(Uint16 type, int num, void *data, size_t size);

//	!!! Utility that sets the binding internally and writes to userdata
//	
//	NOT IMPLEMENTED! DON'T USE!
//	
//	Note: This also clears any bindings that use the same keycode `kc`
//	Returns if Binding or UserData hasn't been initialised.
//	Also returns if either input is invalid (`INPUT_NONE` or `SDLK_UNKNOWN`)
void UserData_SetBinding(Input_Type in, SDL_KeyCode kc);

#endif