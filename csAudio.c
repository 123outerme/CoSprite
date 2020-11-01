#include "csAudio.h"

int initCoSpriteAudio(int maxChannels)
{
    int status = 0;

    int sampleRate = 22050; //in Hz. Double this if the target computer is powerful

    if (Mix_OpenAudio(sampleRate, MIX_DEFAULT_FORMAT, 2/* 1 = mono, 2 = stereo */, 1024/* bytes per output sample */) == -1)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", SDL_GetError());
        status = 1;  //this isn't a fatal error
        globalAudio.musicVolume = 0;
        globalAudio.soundVolume = 0;
        globalAudio.maxChannels = 0;
    }
    else
    {
        //Mix_Init(MIX_INIT_OGG);  //deprecated?
        globalAudio.soundVolume = MIX_MAX_VOLUME;
        globalAudio.maxChannels = maxChannels;
        Mix_AllocateChannels(maxChannels);
        Mix_Volume(-1, globalAudio.soundVolume);  //sets all channels to the sound level global.soundVolume
        globalAudio.musicVolume = MIX_MAX_VOLUME;
        Mix_VolumeMusic(globalAudio.musicVolume);
    }
    return status;
}

void initCSMusic(csMusic* music, char* filepath, void (*musicFinished)())
{
    music->music = Mix_LoadMUS(filepath);

    int filepathLen = strlen(filepath) + 1;
    music->filepath = calloc(filepathLen, sizeof(char));
    strncpy(music->filepath, filepath, filepathLen);

    music->musicFinished = musicFinished;
}

void playCSMusic(csMusic* music, int loops)
{
    Mix_PlayMusic(music->music, loops);
    Mix_HookMusicFinished(music->musicFinished);
}

void destroyCSMusic(csMusic* music)
{
    Mix_FreeMusic(music->music);
    free(music->filepath);
    music->musicFinished = NULL;
}

void initCSSoundFX(csSoundFX* sound, char* filepath)
{
    sound->sound = Mix_LoadWAV(filepath);

    int filepathLen = strlen(filepath) + 1;
    sound->filepath = calloc(filepathLen, sizeof(char));
    strncpy(sound->filepath, filepath, filepathLen);

    sound->curChannel = -1;
}

void playCSSoundFX(csSoundFX* sound, int loops)
{
    sound->curChannel = Mix_PlayChannel(-1, sound->sound, loops);
}

void destroyCSSoundFX(csSoundFX* sound)
{
    Mix_FreeChunk(sound->sound);
    free(sound->filepath);
}

//Mix_PlayMusic(MUSIC((musicIndex = 1)), -1);

void closeCoSpriteAudio()
{
    globalAudio.musicVolume = 0;
    globalAudio.soundVolume = 0;
    globalAudio.maxChannels = 0;

    Mix_CloseAudio();
}
