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

    /** \brief Initializes a pSprite object.
     *
     * \param sprite - a pointer to your sprite.
     * \param texture - a SDL_Texture* that holds your sprite's image
     * \param x - x position onscreen
     * \param y - y position onscreen
     * \param w - width of your sprite
     * \param h - height of your sprite
     * \param scale - size * this == drawn size
     * \param subclass - void*. Do with it what you will, isn't used internally
     */
void initPSprite(pSprite* sprite, SDL_Texture* texture, int x, int y, int w, int h, double scale, void* subclass)
{
    sprite->texture = texture;
    sprite->x = x;
    sprite->y = y;
    sprite->w = w;
    sprite->h = h;
    sprite->scale = scale;
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

/** \brief Creates a file, or clears contents if it exists.
 *
 * \param filePath - valid string filepath (relative or absolute)
 * \return Error code: Code 0: No error. Code -1: Error opening
 */
int createFile(char* filePath)
{
	FILE* filePtr;
	filePtr = fopen(filePath,"w");
	if (!filePtr)
	{
		printf("Error opening/creating file!\n");
		return -1;
	}
	else
    {
        fclose(filePtr);
		return 0;
    }
}

/** \brief Checks if a file exists and if it has certain number of lines.
 *
 * \param filePath - valid string filepath (relative or absolute)
 * \param desiredLines - Compares this number to actual lines. If desiredLines < 0, gets number of lines instead.
 * \return 1 if desiredLines >= 0 and desiredLines >= lines. 0 otherwise. If desiredLines < 0, returns number of lines instead.
 */
int checkFile(char* filePath, int desiredLines)
{
    FILE* filePtr = fopen(filePath, "r");
	if (!filePtr)
		return false;
    char ch;
    int lines = 0;
    while(!feof(filePtr))
    {
      ch = fgetc(filePtr);
      if(ch == '\n')
      {
        lines++;
      }
    }
    fclose(filePtr);
    return desiredLines >= 0 ? lines >= desiredLines : lines;
}

/** \brief Adds a line of text to the end of a file
 *
 * \param filePath - valid string filepath (relative or absolute)
 * \param stuff - string containing desired text.
 * \return Error code: Code 0: No error. Code -1: Error opening file
 */
int appendLine(char* filePath, char* stuff)
{
	FILE* filePtr;
	filePtr = fopen(filePath,"a");
	if (!filePtr)
	{
		printf("Error opening file!\n");
		return -1;
	}
	else
	{
		fprintf(filePtr, "%s\n", stuff);
		fclose(filePtr);
		return 0;
	}
}

/** \brief Reads a line of a file.
 *
 * \param filePath - valid string filepath (relative or absolute)
 * \param lineNum - the line number (starting from 0)
 * \param maxLength - how long the string should be, max.
 * \param output - valid pointer to your char* (should not be read-only)
 * \return NULL if it fails, otherwise your string
 */
char* readLine(char* filePath, int lineNum, int maxLength, char** output)
{
	FILE* filePtr = fopen(filePath,"r");
	if (!filePtr)
		return NULL;
	else
	{
        char* thisLine = calloc(maxLength, sizeof(char));
        fseek(filePtr, 0, SEEK_SET);
        for(int p = 0; p <= lineNum; p++)
            fgets(thisLine, maxLength, filePtr);
        //printf("%s @ %d\n", thisLine, thisLine);
        strncpy(*output, thisLine, maxLength);
        //printf("%s @ %d\n", output, output);
        fclose(filePtr);
        free(thisLine);
        return *output;
	}
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
