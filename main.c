#include "csGraphics.h"
#include "csInput.h"
#include "csUtility.h"
#include "csCurl.h"
#include "csMap.h"

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

void loadSprite(cSprite* sprite, char* filePath, cDoubleRect rect, cDoubleRect clipRect, cDoublePt* center, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int priority);
int cMenu(cSprite cursor, char* title, char** optionsArray, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, void (*extraDrawing)(void));

int main(int argc, char* argv[])
{
    argv[argc - 1] = " ";  //just to get rid of warnings
    int code = initCoSprite("cb.bmp", "CoSprite Test/Example", 960, 480, "Px437_ITT_BIOS_X.ttf", 24, 5, (SDL_Color) {255, 28, 198, 0xFF}, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    initCoSpriteCurl(CURL_GLOBAL_ALL);

    randomNums = (intArray) {calloc(MAX_RAND_NUMS, sizeof(int)), 0};
    int randInts[3] = {0, 0, 0};

    for(int i = 0; i < 3; i++)
        randInts[i] = getRandInt();

    char* randString = calloc(10, sizeof(char));
    snprintf(randString, 9, "%d.%d.%d", randInts[0], randInts[1], randInts[2]);

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

    /*bool quit = false;
    SDL_Keycode key;
    char filename[FILENAME_MAX - 1] = " ";
    cText fileText;
    cCamera cam;
    cScene fileScene;
    for(int i = 1; i < FILENAME_MAX; i++)
        filename[i] = '\0';
    initCCamera(&cam, (cDoubleRect) {0, 0, 20, 10}, 1.0, 0);
    initCText(&fileText, filename, (cDoubleRect) {0, 0, global.windowW, global.windowH}, (SDL_Color) {0, 0, 0, 0xFF}, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, SDL_FLIP_NONE, 0, true, 5);
    initCScene(&fileScene, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, &cam, NULL, 0, NULL, 0, NULL, 0, (cText*[1]) {&fileText}, 1);
    while(!quit)
    {
        key = getKey(false);
        handleTextInput(filename, key, FILENAME_MAX);
        fileText.string = filename;
        drawCScene(&fileScene, true, true);
        if (key == SDLK_RETURN || key == SDLK_ESCAPE || key == -1)
            quit = true;
    }
    printf("%s\n", filename);*/
    cSprite lowerSprite, upperSprite;
    loadSprite(&lowerSprite, "cb.bmp", (cDoubleRect) {50, 50, 120, 150}, (cDoubleRect) {0, 0, 120, 150}, NULL, 1.0, SDL_FLIP_NONE, 0, false, NULL, 4);
    loadSprite(&upperSprite, "cb.bmp", (cDoubleRect) {0, 0, 150, 120}, (cDoubleRect) {0, 0, 150, 120}, NULL, 1.0, SDL_FLIP_NONE, 0, false, NULL, 5);
    cText txt;
    initCText(&txt, randString, (cDoubleRect) {150, 150, 300, 300}, (SDL_Color) {0, 0, 0, 0xFF}, (SDL_Color) {0xFF, 0, 0, 0x00}, 1.0, SDL_FLIP_NONE, 0, false, 1);
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
    initCScene(&scene, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, &camera, (cSprite*[2]) {&lowerSprite, &upperSprite}, 0, /*(c2DModel*[1]) {&model}*/NULL, 0, (cResource**) NULL, 0, (cText*[1]) {&txt}, 1);
    add2DModelToCScene(&scene, &model);
    SDL_Keycode key;

    while(!(key == -1 || key == SDLK_ESCAPE))
    {
        key = getKey(false);
        if (key == SDLK_w)
        {
            model.rect.y -= 8;
            if (scene.modelCount == 0)
            {
                lowerSprite.drawRect.y -= 8;
                upperSprite.drawRect.y -= 8;
            }
        }

        if (key == SDLK_s)
        {
            model.rect.y += 8;
            if (scene.modelCount == 0)
            {
                lowerSprite.drawRect.y += 8;
                upperSprite.drawRect.y += 8;
            }
        }

        if (key == SDLK_a)
        {
            model.rect.x -= 8;
            if (scene.modelCount == 0)
            {
                lowerSprite.drawRect.x -= 8;
                upperSprite.drawRect.x -= 8;
            }
        }

        if (key == SDLK_d)
        {
            model.rect.x += 8;
            if (scene.modelCount == 0)
            {
                lowerSprite.drawRect.x += 8;
                upperSprite.drawRect.x += 8;
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

        if (key == SDLK_F12)
            printf("%f, %f center %f, %f\n", model.rect.x, model.rect.y, model.rect.x + model.rect.w / 2, model.rect.y + model.rect.h / 2);
        drawCScene(&scene, true, true);
    }
    free(randString);
    free(randomNums.arr);
    exportC2DModel(&model, "exported.bin");
    destroyCScene(&scene);
    closeCoSprite();
    closeCoSpriteCurl();
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
