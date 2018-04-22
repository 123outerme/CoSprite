#include "pistonMain.h"

#define IMG_INIT_FLAGS IMG_INIT_PNG

/** \brief Initializes an SDL window and all of Piston's inner stuff.
 * \return Error code: Code 0: No error. Code 1: SDL systems failed to initialize. Code 2: Window could not be created Code 3: Renderer failed to initialize
 */
int initPiston(char* iconPath, char* windowName, int windowWidth, int windowHeight, char* fontPath, int fontSize)
{
    int status = 0;
    mainWindow = NULL;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    else
    {
        if(!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_FLAGS))
        {
            printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
            return 1;
        }
        //Initialize SDL_ttf
        if(TTF_Init() == -1)
        {
            printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
            return 1;
        }
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        {
            printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", SDL_GetError());
            return 1;
        }
        else
        //Mix_Init(MIX_INIT_OGG);  //deprecated?
        soundVolume = MIX_MAX_VOLUME;
        Mix_AllocateChannels(32);
        Mix_Volume(-1, soundVolume);  //sets all channels to the sound level soundVolume
        musicVolume = MIX_MAX_VOLUME;
        Mix_VolumeMusic(musicVolume);
        mainScreen = NULL;
        mainRenderer = NULL;
        mainFont = NULL;
        mainWindow = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        if (!mainWindow)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return 2;
        }
        else
        {
            windowW = windowWidth;
            windowH = windowHeight;
            mainScreen = SDL_GetWindowSurface(mainWindow);
            mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
            if(!mainRenderer)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                return 3;
            }
            else
            {
                SDL_Surface* iconSurface = IMG_Load(iconPath);
                SDL_SetWindowIcon(mainWindow, iconSurface);
                SDL_FreeSurface(iconSurface);
                SDL_SetRenderDrawBlendMode(mainRenderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderSetLogicalSize(mainRenderer, windowWidth, windowHeight);
                SDL_RenderClear(mainRenderer);
                loadTTFont(fontPath, &mainFont, fontSize);
                //loadTTFont(FONT_FILE_NAME, &smallFont, 20);
                if (!mainFont)
                {
                    canDrawText = false;
                    status = 4;
                }

                srand((unsigned int) time(NULL));
                /*if (checkFile(CONFIG_FILE_NAME, SIZE_OF_SCANCODE_ARRAY))
                {
                    loadConfig(CONFIG_FILE_NAME);
                }
                else
                    initConfig(CONFIG_FILE_NAME);*/
            }
        }
    }
    return status;
}

/** \brief Loads an image into a SDL_Texture*
 *
 * \param imgPath - valid string filepath (relative or absolute)
 * \param dest - pointer to your SDL_Texture*
 * \return true if created, false otherwise
 */
bool loadIMG(char* imgPath, SDL_Texture** dest)
{
    SDL_Surface* surf = IMG_Load(imgPath);
    if (!surf)
    {
	printf("Unable to load image for %s! SDL_Error: %s\n", imgPath, SDL_GetError());
        return false;
    }
    SDL_SetColorKey(surf, 1, SDL_MapRGB(surf->format, 255, 28, 198));
    *dest = SDL_CreateTextureFromSurface(mainRenderer, surf);
    if (!(*dest))
    {
        printf("Unable to create texture for %s! SDL_Error: %s\n", imgPath, SDL_GetError());
        return false;
    }
    SDL_FreeSurface(surf);
    return true;
}

/** \brief Loads a .ttf font
 *
 * \param filePath - valid string filepath (relative or absolute)
 * \param dest - pointer to your SDL_Texture*
 * \param sizeInPts - font size, measured in points
 * \return true if created, false otherwise
 */
bool loadTTFont(char* filePath, TTF_Font** dest, int sizeInPts)
{
    *dest = TTF_OpenFont(filePath, sizeInPts);
    if (!*dest)
    {
        printf("Font could not be loaded! SDL Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

/** \brief Initializes a pSprite object. You may want to create a wrapper method.
 *
 * \param sprite - a pointer to your sprite.
 * \param texture - a SDL_Texture* that holds your sprite's image
 * \param x - x position onscreen
 * \param y - y position onscreen
 * \param w - width of your sprite
 * \param h - height of your sprite
 * \param scale - size * this == drawn size
 * \param flip - SDL_RenderFlip value
 * \param degrees - rotation angle in degrees
 * \param subclass - void*. Do with it what you will, isn't used internally
 */
void initPSprite(pSprite* sprite, SDL_Texture* texture, SDL_Rect rect, double scale, SDL_RendererFlip flip, double degrees, void* subclass)
{
    sprite->texture = texture;
    sprite->rect = rect;
    sprite->scale = scale;
    sprite->degrees = degrees;
    sprite->flip = flip;
    sprite->subclass = subclass;
}

/** \brief gets a keypress
 *
 * \return key you pressed as an SDL_Keycode, or -1 if a quit signal was sent
 */
SDL_Keycode getKey()
{
    SDL_Event e;
    SDL_Keycode keycode = 0;
    while(SDL_PollEvent(&e) != 0)
    {
        if(e.type == SDL_QUIT)
            keycode = -1;
        else
            if(e.type == SDL_KEYDOWN)
                keycode = e.key.keysym.sym;
    }
    return keycode;
}

/** \brief Just like getKey(), except it waits
 *
 * \return key you pressed as an SDL_Keycode, or -1 if a quit signal was sent
 */
SDL_Keycode waitForKey()
{
    SDL_Keycode key;
    while(!(key = getKey()));
    return key;
}

/** \brief draws a pSprite to the screen
 *
 * \param sprite - pSprite you want drawn
 */
void drawPSprite(pSprite sprite, bool update)
{
    SDL_RenderCopyEx(mainRenderer, sprite.texture, &((SDL_Rect) {.x = 0, .y = 0, .w = sprite.rect.w, .h = sprite.rect.h}), &((SDL_Rect) {.x = sprite.rect.x, .y = sprite.rect.y, .w = sprite.rect.w * sprite.scale, .h = sprite.rect.h * sprite.scale}), sprite.degrees, NULL, sprite.flip);
    if (update)
        SDL_RenderPresent(mainRenderer);
}

/** \brief converts any int to a string.
 *
 * \param value - what value you want
 * \param result - valid pointer to get your string
 * \return pointer to your inputted string (for convenience)
 */
char* intToString(int value, char* result)
{
    if (value == 0)
        return "0";
    bool negFlag = false;
    if (value < 0)
    {
        negFlag = true;
        value *= -1;
        //printf("new value = %d\n", value);
    }
	int digit = digits(value);
	//printf("digit = %d\n", digit);
	result = calloc(digit + 1 + negFlag, sizeof(char));
	result[digit + negFlag] = '\0';
	int usedVal = 0;
	for (int i = digit; i > 0; i--)
	{
		int x = (value - usedVal) / (int) pow(i - 1, 10);
		result[digit - i] = (char) x + '0';
		//printf("result[%d] = (%d) / %d = %d = character %c\n", digit - i, value - usedVal, pwrOf10(i - 1), x, result[digit - i]);
		usedVal = usedVal + (result[digit - i] - '0') * (int) pow(i - 1, 10);
		//printf("usedVal = itself + %d * %d = %d\n", (int) result[digit - i] - '0', pwrOf10(i - 1), usedVal);
	}
	if (negFlag)
    {
        char negative[digit + 1];
        negative[0] = '-';
        strcat(negative, result);
        strcpy(result, negative);
    }
	//printf("%s\n",result);
	return result;
}

/** \brief Gets the amount of digits in num.
 *
 * \param num - your number
 * \return the number of digits in num.
 */
int digits(int num)
{
    if (num < 0)
        num *= -1;
	return 1 + log10(num);
}

/** \brief free() with automatic NULL setting.
 * Use: x = freeThisMem(x);
 * \param x - memory address
 * \return NULL
 *
 */
void* freeThisMem(void* x)
{
	free(x);
	return NULL;
}

char* removeNewline(char* stuff, char replacement, int maxLength)
{
    for(int i = maxLength - 1; i >= 0; i--)
    {
        if (i < 0)
            return stuff;
        if (stuff[i] == '\n')
        {
            stuff[i] = replacement;
        }
    }
    return stuff;
}
