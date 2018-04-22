#ifndef CRANKSHAFTMAIN_H_INCLUDED
#define CRANKSHAFTMAIN_H_INCLUDED

/* ++ CRANKSHAFT Engine version 1.0 - last update 4/21/2018 ++
  -- initCRANKSHAFT() error codes:  --
  error code 0: No error
  error code 1: SDL systems failed to initialize
  error code 2: Window could not be created
  error code 3: Renderer failed to initialize
*/

//#includes:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include "SDL/SDL.h"       //This is included because it's an SDL2 program... duh
#include "SDL/SDL_image.h" //This is included so we can use PNGs.
#include "SDL/SDL_ttf.h"   //This is included for text stuff
#include "SDL/SDL_mixer.h" //This is included for audio

//#defines:
#define bool char
#define false 0
#define true 1
#define printBool(bool) (bool ? "true" : "false")

//struct definitions:
typedef struct _pSprite {
    SDL_Texture* texture;
    int id;
    SDL_Rect rect;
    double scale;
    SDL_RendererFlip flip;
    double degrees;
    void* subclass;  //fill with any extraneous data or pointer to another struct
} pSprite;

//method prototypes:
int initCrankshaft();
bool loadIMG(char* imgPath, SDL_Texture** dest);
bool loadTTFont(char* filePath, TTF_Font** dest, int sizeInPts);
void initPSprite(pSprite* sprite, SDL_Texture* texture, SDL_Rect rect, double scale, SDL_RendererFlip flip, double degrees, void* subclass);
SDL_Keycode getKey();
SDL_Keycode waitForKey();
void drawPSprite(pSprite sprite, bool update);

char* intToString(int value, char * result);
int digits(int num);
void* freeThisMem(void* x);
char* removeNewline(char* stuff, char replacement, int maxLength);

//global variable declarations:
SDL_Window* mainWindow;
SDL_Surface* mainScreen;
SDL_Renderer* mainRenderer;
TTF_Font* mainFont;

int windowW, windowH;
bool canDrawText;
int soundVolume, musicVolume;

#endif // CRANKSHAFTMAIN_H_INCLUDED
