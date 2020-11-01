#include "csGraphics.h"
#include "csInput.h"
#include "csUtility.h"
#include "csCurl.h"
#include "csMap.h"
#include "csAudio.h"

typedef struct _intArray
{
    int* arr;
    int length;
} intArray;

size_t randIntCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
void getNewRandInts(intArray* arr);
int getRandInt();

intArray randomNums;
int getRandIntPlace = 0;  //please don't modify
const int MAX_RAND_NUMS = 15;

cLogger testLogger;

void loadSprite(cSprite* sprite, char* filePath, cDoubleRect rect, cDoubleRect clipRect, cDoublePt* center, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int priority);
int cMenu(cSprite cursor, char* title, char** optionsArray, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, void (*extraDrawing)(void));
void testMusicCallback();

int main(int argc, char* argv[])
{
    argv[argc - 1] = " ";  //just to get rid of warnings
    int code = initCoSprite("assets/cb.bmp", "CoSprite Test/Example", 960, 480, "assets/Px437_ITT_BIOS_X.ttf", 24, 5, (SDL_Color) {255, 28, 198, 0xFF}, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    initCoSpriteCurl(CURL_GLOBAL_ALL, "./assets/cacert.pem", true);
    initCoSpriteAudio(32);
    csSoundFX testSound;
    initCSSoundFX(&testSound, "./assets/audio/gateway1.ogg");
    playCSSoundFX(&testSound, 0);

    csMusic testMusic;
    initCSMusic(&testMusic, "./assets/audio/mainTheme.mp3", testMusicCallback);
    playCSMusic(&testMusic, 0);

    initCLogger(&testLogger, "./logs/log.txt", NULL);
    cLogEvent(testLogger, "TEST", "Testing logs", "Initialized");

    randomNums = (intArray) {calloc(MAX_RAND_NUMS, sizeof(int)), 0};
    int randInts[3] = {0, 0, 0};

    for(int i = 0; i < 3; i++)
        randInts[i] = getRandInt();

    char* randString = calloc(10, sizeof(char));
    snprintf(randString, 9, "%d.%d.%d", randInts[0], randInts[1], randInts[2]);

    /* get from webpage
    char* webString = calloc(5192, sizeof(char));

    int start = SDL_GetTicks();
    csCurlPerformEasyGet(&globalCurl, "https://gtlmappacks.firebaseio.com/files/mappacks/MainAdventure.json", webString);
    int time = SDL_GetTicks();
    printf("%d elapsed ms\n", time - start);
    //get a string from a website, probably my GtL website

    printf("\n%s\n", webString);

    csMap mappackMap;
    jsonToCSMap(&mappackMap, webString);

    printf("\n%s\n", csMapToJson(mappackMap));

    printf("\n%s\n", traverseCSMapByKey(mappackMap, "likes"));

    free(webString);
    destroyCSMap(&mappackMap);
    */

    /* key input test
    bool quit = false;
    SDL_Keycode testKey;
    char filename[FILENAME_MAX - 1] = " ";
    cText fileText;
    cCamera cam;
    cScene fileScene;
    for(int i = 1; i < FILENAME_MAX; i++)
        filename[i] = '\0';
    initCCamera(&cam, (cDoubleRect) {0, 0, 20, 10}, 1.0, 0);
    initCText(&fileText, filename, (cDoubleRect) {0, 0, global.windowW, global.windowH}, (SDL_Color) {0, 0, 0, 0xFF}, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, 0, SDL_FLIP_NONE, 0, true, 5);
    initCScene(&fileScene, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, &cam, NULL, 0, NULL, 0, NULL, 0, (cText*[1]) {&fileText}, 1);
    while(!quit)
    {
        testKey = getKey(false);
        handleTextInput(filename, testKey, FILENAME_MAX);
        updateCText(&fileText, filename);
        drawCScene(&fileScene, true, true, NULL);
        if (testKey == SDLK_RETURN || testKey == SDLK_ESCAPE || testKey == -1)
            quit = true;
    }
    printf("%s\n", filename);
    //*/

    /* line collision test
    bool done = false;
    while(!done)
    {
        bool quit = false;
        cInputState inputs;
        cDoubleLine testLine1, testLine2;
        int count = 0;

        while(!quit)
        {
            SDL_RenderClear(global.mainRenderer);
            inputs = cGetInputState(true);
            if (inputs.isClick)
            {
                if (count == 0)
                {
                    testLine1.x1 = inputs.click.x;
                    testLine1.y1 = inputs.click.y;
                }
                if (count == 1)
                {
                    testLine1.x2 = inputs.click.x;
                    testLine1.y2 = inputs.click.y;
                }
                if (count == 2)
                {
                    testLine2.x1 = inputs.click.x;
                    testLine2.y1 = inputs.click.y;
                }
                if (count == 3)
                {
                    testLine2.x2 = inputs.click.x;
                    testLine2.y2 = inputs.click.y;
                }
                count++;
                if (count == 4)
                    quit = true;
            }

            SDL_SetRenderDrawColor(global.mainRenderer, 0xFF, 0x00, 0x00, 0xFF);

            if (count > 0)
                SDL_RenderDrawPoint(global.mainRenderer, testLine1.x1, testLine1.y1);
            if (count > 1)
                SDL_RenderDrawLine(global.mainRenderer, testLine1.x1, testLine1.y1, testLine1.x2, testLine1.y2);

            if (count > 2)
                SDL_RenderDrawPoint(global.mainRenderer, testLine2.x1, testLine2.y1);

            if (count > 3)
                SDL_RenderDrawLine(global.mainRenderer, testLine2.x1, testLine2.y1, testLine2.x2, testLine2.y2);

            SDL_SetRenderDrawColor(global.mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

            SDL_RenderPresent(global.mainRenderer);
        }
        // get coords

        printf("%s\n", boolToString(checkIntersection(testLine1, testLine2)));

        SDL_Keycode keycode = waitForKey(true);
        done = (keycode == SDLK_ESCAPE || keycode == -1);
    }
    //*/

    cSprite lowerSprite, upperSprite;
    loadSprite(&lowerSprite, "assets/cb.bmp", (cDoubleRect) {50 / 48.0, 50 / 48.0, 120 / 48.0, 150 / 48.0}, (cDoubleRect) {0, 0, 120, 150}, NULL, 1.0, SDL_FLIP_NONE, 0, false, NULL, 4);
    loadSprite(&upperSprite, "assets/cb.bmp", (cDoubleRect) {0, 0, 150 / 48.0, 120 / 48.0}, (cDoubleRect) {0, 0, 150, 120}, NULL, 1.0, SDL_FLIP_NONE, 0, false, NULL, 5);
    cText txt;
    initCText(&txt, randString, (cDoubleRect) {150 / 48.0, 150 / 48.0, 300, 300}, 300, (SDL_Color) {0, 0, 0, 0xFF}, (SDL_Color) {0xFF, 0, 0, 0xFF}, NULL, 1.0, SDL_FLIP_NONE, 0, false, 1);
    c2DModel model;
    if (checkFile("exported.bin") > 0)
    {
        importC2DModel(&model, "exported.bin");
    }
    else
    {
        initC2DModel(&model, (cSprite[2]) {lowerSprite, upperSprite}, 2, (cDoublePt) {0, 0}, NULL, 1.0, SDL_FLIP_NONE, 0.0, false, NULL, 5);
    }
    cCamera camera;
    initCCamera(&camera, (cDoubleRect) {0, 0, global.windowW / 48, global.windowH / 48}, 1.0, 0.0);
    cScene scene;
    initCScene(&scene, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, &camera, /*(cSprite*[2]) {&lowerSprite, &upperSprite}*/ NULL, 0, /**/(c2DModel*[1]) {&model}/**/, 1, (cResource**) NULL, 0, (cText*[1]) {&txt}, 1);
    //add2DModelToCScene(&scene, &model);
    SDL_Keycode key;
    startTime = SDL_GetTicks();
    int framerate = 0;

    while(!(key == -1 || key == SDLK_ESCAPE))
    {
        key = getKey(false);
        if (key == SDLK_w)
        {
            model.rect.y -= 1;
            if (scene.modelCount == 0)
            {
                lowerSprite.drawRect.y -= 1;
                upperSprite.drawRect.y -= 1;
            }
        }

        if (key == SDLK_s)
        {
            model.rect.y += 1;
            if (scene.modelCount == 0)
            {
                lowerSprite.drawRect.y += 1;
                upperSprite.drawRect.y += 1;
            }
        }

        if (key == SDLK_a)
        {
            model.rect.x -= 1;
            if (scene.modelCount == 0)
            {
                lowerSprite.drawRect.x -= 1;
                upperSprite.drawRect.x -= 1;
            }
        }

        if (key == SDLK_d)
        {
            model.rect.x += 1;
            if (scene.modelCount == 0)
            {
                lowerSprite.drawRect.x += 1;
                upperSprite.drawRect.x += 1;
            }
        }

        if (key == SDLK_DELETE)
        {
            remove2DModelFromCScene(&scene, &model, -1, false);
        }

        if (key == SDLK_p)
        {
            for(int i = 0; i < 3; i++)
                randInts[i] = getRandInt();

            snprintf(randString, 9, "%d.%d.%d", randInts[0], randInts[1], randInts[2]);
            updateCText(&txt, randString);
        }

        if (key == SDLK_UP)
            camera.rect.y--;
        if (key == SDLK_DOWN)
            camera.rect.y++;
        if (key == SDLK_LEFT)
            camera.rect.x--;
        if (key == SDLK_RIGHT)
            camera.rect.x++;

        if (key == SDLK_q)
            camera.degrees -= 10;
        if (key == SDLK_e)
            camera.degrees += 10;

        if (key == SDLK_z)
            model.degrees -= 10;
        if (key == SDLK_x)
            model.degrees += 10;
        if (key == SDLK_c)
            camera.zoom -= .1;
        if (key == SDLK_v)
            camera.zoom += .1;

        if (key == SDLK_9)
        {
            lowerSprite.scale -= .1;
            upperSprite.scale -= .1;
            model.sprites[0].scale -= .1;
            model.sprites[1].scale -= .1;
        }
        if (key == SDLK_0)
        {
            lowerSprite.scale += .1;
            upperSprite.scale += .1;
            model.sprites[0].scale += .1;
            model.sprites[1].scale += .1;
        }

        if (key == SDLK_MINUS)
        {
            lowerSprite.degrees -= 10;
            upperSprite.degrees -= 10;
            model.sprites[0].degrees -= 10;
            model.sprites[1].degrees -= 10;
        }
        if (key == SDLK_EQUALS)
        {
            lowerSprite.degrees += 10;
            upperSprite.degrees += 10;
            model.sprites[0].degrees += 10;
            model.sprites[1].degrees += 10;
        }

        if (key == SDLK_F11)
            printf("%d\n", framerate);

        if (key == SDLK_F12)
            printf("%f, %f center %f, %f\n", model.rect.x, model.rect.y, model.rect.x + model.rect.w / 2, model.rect.y + model.rect.h / 2);


        drawCScene(&scene, true, true, &framerate, 60);
    }
    free(randString);
    free(randomNums.arr);
    exportC2DModel(&model, "exported.bin");
    destroyCScene(&scene);
    destroyCLogger(&testLogger);

    destroyCSSoundFX(&testSound);
    destroyCSMusic(&testMusic);

    closeCoSpriteAudio();
    closeCoSpriteCurl();
    closeCoSprite();

    return code;
}

void loadSprite(cSprite* sprite, char* filePath, cDoubleRect rect, cDoubleRect clipRect, cDoublePt* center, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int priority)
{
    SDL_Texture* tempTexture;
    loadIMG(filePath, &tempTexture);
    if (!center)
        center = &((cDoublePt) {rect.w / 2, rect.h / 2});
    initCSprite(sprite, tempTexture, filePath, 1, rect, clipRect, center, scale, flip, degrees, fixed, subclass, priority);
}

/** \brief Draws a standard menu
* \param texture - the texture to use for drawing the cursor
* \param cursorRect - where on the texture the cursor is
* \param title - the title text for the menu
* \param optionsArray - (char*[x]) {...} filled with all options
* \param options - number of options (max 9)
* \param curSelect - preselect an option (1-9)
* \param bgColor - color of the screen background
* \param titleColorUnder - color of the title text shadow
* \param titleColorOver - color of the title text
* \param textColor - color of the options
* \param border - draws a bgColor-colored border if nonzero
* \param extraDrawing - draws whatever is in the function after all other text have been drawn
* \return the option chosen (1-9 or -1 if force-quit)
*/
int cMenu(cSprite cursor, char* title, char** optionsArray, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, void (*extraDrawing)(void))
{
    const int ANYWHERE_QUIT = -1, SCREEN_WIDTH = global.windowW, SCREEN_HEIGHT = global.windowH, TILE_SIZE = 48, MAX_ITEMS = 9, HEIGHT_IN_TILES = SCREEN_HEIGHT / TILE_SIZE;
    if (curSelect < 1)
        curSelect = 1;
    if (options < 0)
        return ANYWHERE_QUIT;
    cursor.drawRect = (cDoubleRect) {.x = TILE_SIZE, .y = (curSelect + 4) * TILE_SIZE, .w = TILE_SIZE, .h = TILE_SIZE};
    SDL_Event e;
    bool quit = false;
    int selection = -1;
    //While application is running
    while(!quit)
    {
        if (border)
            SDL_SetRenderDrawColor(global.mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);
        else
            SDL_SetRenderDrawColor(global.mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);

        SDL_RenderClear(global.mainRenderer);
        SDL_RenderFillRect(global.mainRenderer, NULL);
        SDL_SetRenderDrawColor(global.mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
        SDL_RenderFillRect(global.mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
        //background text (drawn first)
        drawText(title, TILE_SIZE + TILE_SIZE / 4, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, titleColorUnder, false);
        //foreground text
        drawText(title, TILE_SIZE + TILE_SIZE / 4, 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, titleColorOver, false);

        for(int i = 0; ((options <= MAX_ITEMS) ? i < options : i < MAX_ITEMS); i++)
	        drawText(optionsArray[i], 2.25 * TILE_SIZE, (5 + i) * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - (5 + i)) * TILE_SIZE, textColor, false);
        if (extraDrawing)
            (*extraDrawing)();

        //SDL_RenderFillRect(global.mainRenderer, &((SDL_Rect){.x = cursor.x, .y = cursor.y, .w = cursor.w, .h = cursor.w}));
        //Handle events on queue
        while(SDL_PollEvent(&e) != 0)
        {
            //User requests quit
            if(e.type == SDL_QUIT)
            {
                quit = true;
                selection = ANYWHERE_QUIT;
            }
            //User presses a key
            else
            {
                if(e.type == SDL_KEYDOWN)
                {
                    const Uint8* keyStates = SDL_GetKeyboardState(NULL);
                    if (keyStates[KEY_UP] && cursor.drawRect.y > 5 * TILE_SIZE)
                    {
                        cursor.drawRect.y -= TILE_SIZE;
                    }

                    if (keyStates[KEY_DOWN] && cursor.drawRect.y < (options + 4) * TILE_SIZE)
                    {
                        cursor.drawRect.y += TILE_SIZE;
                    }

                    if (keyStates[KEY_CONFIRM])
                    {
                        selection = cursor.drawRect.y / TILE_SIZE - 4;
                        quit = true;
                    }
                }
                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
                {

                    int choice = (e.button.y / TILE_SIZE) - 4;
                    if (choice > 0 && choice <= options)
                    {
                        selection = choice;
                        quit = true;
                    }
                }
                if (e.type == SDL_MOUSEMOTION)
                {
                    if (e.motion.y / TILE_SIZE > 4 && e.motion.y / TILE_SIZE < options + 5)
                        cursor.drawRect.y = TILE_SIZE * (e.motion.y / TILE_SIZE);
                }
            }
        }
        drawCSprite(cursor, (cCamera) {(cDoubleRect) {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 1.0, 0.0}, true, false);
    }
    return selection;
}

int getRandInt()
{
    if (getRandIntPlace >= randomNums.length)
    {
        printf("Need some new values\n");
        getRandIntPlace = 0;
        randomNums.length = 0;
        getNewRandInts(&randomNums);
    }
    return randomNums.arr[getRandIntPlace++];
}

void getNewRandInts(intArray* arr)
{
    int min = 0, max = 9;
    if (globalCurl.handle && globalCurl.online)
    {
        //check quota from: "https://www.random.org/quota/?format=plain"

        char* randURL = calloc(256, sizeof(char));
        snprintf(randURL, 256 * sizeof(char), "https://www.random.org/integers/?num=%d&min=%d&max=%d&col=1&base=10&format=plain&rnd=new", MAX_RAND_NUMS, min, max);
        printf("Get from:\n%s\n", randURL);

        curl_easy_setopt(globalCurl.handle, CURLOPT_URL, randURL);

        curl_easy_setopt(globalCurl.handle, CURLOPT_WRITEFUNCTION, randIntCallback);

        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(globalCurl.handle, CURLOPT_WRITEDATA, (void*) arr);

        /* Perform the request, res will get the return code */
        globalCurl.retCode = curl_easy_perform(globalCurl.handle);

        /*
        for(int i = 0; i < MAX_RAND_NUMS; i++)
        {
            printf("%d ", arr->arr[i]);
        }
        printf("\n");
        //*/

        /* Check for errors */
        if(globalCurl.retCode != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(globalCurl.retCode));
            globalCurl.online = false;
        }

        free(randURL);
    }

    if (!globalCurl.online)
    {
        for(int i = 0; i < MAX_RAND_NUMS; i++)
        {
            arr->arr[i] = min + (rand() % max);
        }
        arr->length = MAX_RAND_NUMS;
    }
}

size_t randIntCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
    intArray* randInts = (intArray*) userdata;
    char* subString = strtok(ptr, "\n");
    while(subString != NULL)
    {
        randInts->arr[randInts->length] = strtol(subString, NULL, 10);
        randInts->length++;
        subString = strtok(NULL, "\n");
    }
    return size * nmemb;
}

void testMusicCallback()
{
    printf("Music finished.\n");
}
