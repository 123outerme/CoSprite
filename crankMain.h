#ifndef CRANKMAIN_H_INCLUDED
#define CRANKMAIN_H_INCLUDED

/* ++ Crank Engine version 1.0 - last update 4/21/2018 ++
  -- initCrank() error codes:  --
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
typedef struct _cSprite {
    SDL_Texture* texture;
    int id;
    SDL_Rect rect;
    double scale;
    SDL_RendererFlip flip;
    double degrees;
    void* subclass;  //fill with any extraneous data or pointer to another struct
} cSprite;

typedef struct _cResource {
    char* filepath;
    SDL_Texture* texture;
    int w;
    int h;
} cResource;

//method prototypes:
int initCrank();
bool loadIMG(char* imgPath, SDL_Texture** dest);
bool loadTTFont(char* filePath, TTF_Font** dest, int sizeInPts);
void initCSprite(cSprite* sprite, SDL_Texture* texture, SDL_Rect rect, double scale, SDL_RendererFlip flip, double degrees, void* subclass);
void initCResource(cResource* res, char* filepath);
void destroyCResource(cResource* res);
SDL_Keycode getKey();
SDL_Keycode waitForKey();
void drawCSprite(cSprite sprite, bool update);
int* loadTextTexture(char* text, SDL_Texture** dest, int maxW, SDL_Color color, bool isBlended);
void drawText(char* input, int x, int y, int maxW, int maxH, SDL_Color color, bool render);

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

#endif // CRANKMAIN_H_INCLUDED
