#ifndef CSAUDIO_H_INCLUDED
#define CSAUDIO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL2/SDL.h"       //This is included because it's an SDL2 program... duh
#include "SDL2/SDL_mixer.h" //This is included for audio

#ifndef bool
    #define bool char
    #define false 0
    #define true 1
    #define boolToString(bool) (bool ? "true" : "false")
#endif // bool
#ifndef NULL
    #define NULL ((void*) 0)
#endif //NULL

typedef struct _csSoundFX
{
    Mix_Chunk* sound;
    char* filepath;
    int curChannel;
} csSoundFX;

typedef struct _csMusic
{
    Mix_Music* music;
    char* filepath;
    void (*musicFinished)();
} csMusic;

typedef struct _csAudio
{
    int soundVolume;
    int musicVolume;
    int maxChannels; /**< if 0, csAudio is disabled. Otherwise has the amount of audio channels */
} csAudio;

int initCoSpriteAudio(int maxChannels);
void initCSMusic(csMusic* music, char* filepath, void (*musicFinished)());
void playCSMusic(csMusic* music, int loops);
void destroyCSMusic(csMusic* music);
void initCSSoundFX(csSoundFX* sound, char* filepath);
void playCSSoundFX(csSoundFX* sound, int loops);
void destroyCSSoundFX(csSoundFX* sound);
void closeCoSpriteAudio();

csAudio globalAudio;

#endif // CSAUDIO_H_INCLUDED
