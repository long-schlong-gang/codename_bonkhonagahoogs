#ifndef SOUND_H
#define SOUND_H

//	
//	Handles audio stuff beyond what Cynth can handle
//	
//	TODO: Group samples by type, so they can have their own volumes
//	      E.g.: Sound effects, music, ambiance, etc.
//	TODO: Maybe add simple tracker to check how much memory is being used; stats
//	TODO: Improve loading/playback to use less memory

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <screen.h>
#include <log.h>

#include "userdata.h"


////	Constants

#define SOUND_SAMPLE_FREQ 44100
#define SOUND_CHUNK_SIZE 2048


////	Types

typedef enum {
	SFX_DIALOGUE_BEEP,
	SFX_TEST_1,
} Sound_Effect;
#define SFX_COUNT 2

typedef enum {
	OST_TITLE,
	OST_TEST_1,
	OST_TEST_2,
	OST_TEST_3,
} Sound_Music;
#define OST_COUNT 4


////	Globals

extern Mix_Chunk *g_SoundEffects[SFX_COUNT];
extern Mix_Music *g_CurrentMusic;
extern Mix_Music *g_QueuedMusic;


////	Public Functions

//	Initialises the Sound System
//	
//	Sets volume from user data; user data should be loaded first
void Sound_Init();

//	Terminates the Sound System
//	
void Sound_Term();

//	Prepares a sound effect to be played
//	
//	Should be called before the sound can be played
void Sound_SFX_Prepare(Sound_Effect sfx);

//	Plays a single sound effect
//	
//	Prepares the sound if it wasn't already (could introduce latency, though)
//	`channel` can be -1 for the next available channel
void Sound_SFX_Play(Sound_Effect sfx, int channel);

//	Clears a prepared sound to make room for new ones
//	
void Sound_SFX_Clear(Sound_Effect sfx);

//	Clears prepared sounds to make room for new ones
//	
//	TODO: Overhaul audio system; this is weird, I think
void Sound_SFX_ClearAll();

//	Loads a music track and prepares it to start playing
//	
//	If there was already a song in the queue it is cleared
void Sound_OST_QueueTrack(Sound_Music ost);

//	Clears the music queue and unloads the music that was in it
//	
void Sound_OST_ClearQueue();

//	Fades the current song out and the 
//	and plays the next one in the queue
//	
//	If the queued song is NULL, it simply fades out
//	`ms` is fade in/out time. Set to 0 to instantly switch
void Sound_OST_FadeNext(int ms);

//	Toggle whether music is currently playing or not
//	
//	`ms` is a time in ms to fade volume in/out
//	NOTE: Currently blocks for the fade time, so keep it short
//	
//	TODO: Make this less shit, if possible
//	It's quite, buggy, so make sure you don't try switching songs while it's paused, or whatever
void Sound_OST_TogglePause(int ms);


#endif