#include "sound.h"


// TODO: Compress sound more or embed somehow

Mix_Chunk *g_SoundEffects[SFX_COUNT];
Mix_Music *g_CurrentMusic = NULL;
Mix_Music *g_QueuedMusic = NULL;

static const char *__sfx_filenames[SFX_COUNT] = {
	"assets/snd/dit.ogg",		// Dialogue Beep
};

static const char *__ost_filenames[OST_COUNT] = {
	"assets/snd/title.mp3",		// Title Screen Theme
	"assets/snd/arabic_nokia.mp3",
	"assets/snd/baka.mp3",
	"assets/snd/km_blues.mp3",
};


void Sound_Init() {
	char msg[512];

	int succ = Mix_Init(MIX_INIT_OGG | MIX_INIT_MP3);
	if (succ == 0) {
		SDL_snprintf(msg, 512, "Failed to initialise sound system: %s", Mix_GetError());
		Log_Message(LOG_ERROR, msg);
		return;
	}

	succ = Mix_OpenAudio(SOUND_SAMPLE_FREQ, AUDIO_S16SYS, 2, SOUND_CHUNK_SIZE);
	if (succ < 0) {
		SDL_snprintf(msg, 512, "Failed to initialise audio device: %s", Mix_GetError());
		Log_Message(LOG_ERROR, msg);
		return;
	}

	// TODO: Set from user prefs
	Mix_MasterVolume( 0.30f * MIX_MAX_VOLUME );

	Log_Message(LOG_INFO, "Successfully Initialised Sound System!");
}

void Sound_Term() {
	Mix_CloseAudio();
	Mix_Quit();
	Log_Message(LOG_INFO, "Terminated Sound System");
}

void Sound_SFX_Prepare(Sound_Effect sfx) {
	if (sfx < 0 || sfx > SFX_COUNT) {
		char msg[128];
		SDL_snprintf(msg, 128, "Tried to prepare invalid sound effect (0x%04X)", sfx);
		Log_Message(LOG_WARNING, msg);
		return;
	}

	if (g_SoundEffects[sfx] != NULL) return;

	g_SoundEffects[sfx] = Mix_LoadWAV(__sfx_filenames[sfx]);
	if (g_SoundEffects[sfx] == NULL) {
		char msg[512];
		SDL_snprintf(msg, 512, "Failed to prepare sound effect (0x%04X, '%s'): %s", sfx, __sfx_filenames[sfx], Mix_GetError());
		Log_Message(LOG_ERROR, msg);
		return;
	}
}

void Sound_SFX_Play(Sound_Effect sfx, int channel) {
	if (sfx < 0 || sfx > SFX_COUNT) {
		char msg[128];
		SDL_snprintf(msg, 128, "Tried to play invalid sound effect (0x%04X)", sfx);
		Log_Message(LOG_WARNING, msg);
		return;
	}

	if (g_SoundEffects[sfx] == NULL) {
		char msg[128];
		SDL_snprintf(msg, 128, "Played unprepared sound effect (0x%04X)", sfx);
		Log_Message(LOG_WARNING, msg);

		Sound_SFX_Prepare(sfx);
	}

	Mix_PlayChannel(channel, g_SoundEffects[sfx], 0);
}

void Sound_SFX_ClearAll() {
	for (int i=0; i<SFX_COUNT; i++) {
		if (g_SoundEffects[i] == NULL) continue;
		Mix_FreeChunk(g_SoundEffects[i]);
	}
}

void Sound_OST_QueueTrack(Sound_Music ost) {
	if (ost < 0 || ost > OST_COUNT) {
		char msg[128];
		SDL_snprintf(msg, 128, "Tried to queue invalid music track (0x%04X)", ost);
		Log_Message(LOG_WARNING, msg);
		return;
	}

	if (g_QueuedMusic != NULL) Sound_OST_ClearQueue();

	g_QueuedMusic = Mix_LoadMUS(__ost_filenames[ost]);
	if (g_QueuedMusic == NULL) {
		char msg[512];
		SDL_snprintf(msg, 512, "Failed to load music track 0x%04X ('%s'): %s", ost, __ost_filenames[ost], Mix_GetError());
		Log_Message(LOG_ERROR, msg);
		return;
	}
}

void Sound_OST_ClearQueue() {
	if (g_QueuedMusic == NULL) return;

	Mix_FreeMusic(g_QueuedMusic);
	g_QueuedMusic = NULL;
}

void Sound_OST_FadeNext(int ms) {
	g_CurrentMusic = g_QueuedMusic;
	g_QueuedMusic = NULL;

	Mix_FadeOutMusic(ms);
	if (g_CurrentMusic != NULL) Mix_FadeInMusic(g_CurrentMusic, -1, ms);
}

void Sound_OST_TogglePause(int ms) {
	Uint64 start = SDL_GetTicks64();

	if (ms <= 0) {
		if (Mix_PausedMusic()) Mix_ResumeMusic();
		else Mix_PauseMusic();
		return;
	}

	if (Mix_PausedMusic()) {
		Mix_VolumeMusic(0);
		Mix_ResumeMusic();

		int diff = SDL_GetTicks64() - start;
		while (diff < ms) {
			Mix_VolumeMusic( ((float)(diff) / ms) * MIX_MAX_VOLUME);
			SDL_Delay(5);
			diff = SDL_GetTicks64() - start;
		}
	} else {
		int diff = SDL_GetTicks64() - start;
		while (diff < ms) {
			Mix_VolumeMusic( (1.0f - (float)(diff) / ms) * MIX_MAX_VOLUME);
			SDL_Delay(5);
			diff = SDL_GetTicks64() - start;
		}

		Mix_PauseMusic();
		Mix_VolumeMusic(0);
	}
}