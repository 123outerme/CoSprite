#include "csFile.h"
#include "csGraphics.h"
#include "csIO.h"
#include "csUtility.h"

void loadSprite(cSprite* sprite, char* filePath, SDL_Rect rect, SDL_Rect clipRect, SDL_Point center, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int priority);
int cMenu(cSprite cursor, char* title, char** optionsArray, const int options, int curSelect, SDL_Color bgColor, SDL_Color titleColorUnder, SDL_Color titleColorOver, SDL_Color textColor, bool border, void (*extraDrawing)(void));

int main(int argc, char* argv[])
{
    int code = initCoSprite("cb.bmp", "CoSprite Test/Example", 960, 480, "Px437_ITT_BIOS_X.ttf", 24);
    cSprite lowerSprite, upperSprite;
    cText txt;
    loadSprite(&lowerSprite, "cb.bmp", (SDL_Rect) {120, 120, 120, 120}, (SDL_Rect) {0, 0, 120, 120}, (SDL_Point) {0, 0}, 1.0, SDL_FLIP_NONE, 0, false, NULL, 4);
    loadSprite(&upperSprite, "cb.bmp", (SDL_Rect) {0, 0, 120, 120}, (SDL_Rect) {0, 0, 120, 120}, (SDL_Point) {120, 120}, 1.0, SDL_FLIP_NONE, 0, false, NULL, 5);
    initCText(&txt, "Hello world!", (SDL_Rect) {150, 150, 300, 300}, (SDL_Color) {0, 0, 0, 0xFF}, (SDL_Color) {0xFF, 0, 0, 0x00}, SDL_FLIP_NONE, 0, false, 1);
    c2DModel model;
    initC2DModel(&model, (cSprite[2]) {lowerSprite, upperSprite}, 2, (SDL_Point) {0, 0}, NULL, 1.0, SDL_FLIP_NONE, 0.0, false, NULL, 5);
    cCamera camera;
    initCCamera(&camera, (SDL_Rect) {0, 0, windowW / 48, windowH / 48}, 1.0, 0.0);
    cScene scene;
    initCScene(&scene, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, &camera, (cSprite*[2]) {&lowerSprite, &upperSprite}, 0, (c2DModel*[1]) {&model}, 1, (cResource**) NULL, 0, (cText*[1]) {&txt}, 1);
    int key;
    while(key != -1)
    {
        key = getKey(false);
        if (key == SDLK_w)
            model.rect.y -= 8;
        if (key == SDLK_s)
            model.rect.y += 8;
        if (key == SDLK_a)
            model.rect.x -= 8;
        if (key == SDLK_d)
            model.rect.x += 8;

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
        if (key == SDLK_c)
            model.degrees += 10;

        if (key == SDLK_MINUS)
        {
            model.sprites[0].degrees -= 10;
            model.sprites[1].degrees -= 10;
        }
        if (key == SDLK_EQUALS)
        {
            model.sprites[0].degrees += 10;
            model.sprites[1].degrees += 10;
        }

        if (key == SDLK_F12)
            printf("%d, %d center %d, %d\n", model.rect.x, model.rect.y, model.rect.x + model.rect.w / 2, model.rect.y + model.rect.h / 2);
        drawCScene(&scene, true);
    }
    destroyCScene(&scene);
    closeCoSprite();
    return code;
}

void loadSprite(cSprite* sprite, char* filePath, SDL_Rect rect, SDL_Rect clipRect, SDL_Point center, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int priority)
{
    SDL_Texture* tempTexture;
    loadIMG(filePath, &tempTexture);
    initCSprite(sprite, tempTexture, 1, rect, clipRect, &center, scale, flip, degrees, fixed, subclass, priority);
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
    const int ANYWHERE_QUIT = -1, SCREEN_WIDTH = windowW, SCREEN_HEIGHT = windowH, TILE_SIZE = 48, MAX_ITEMS = 9, HEIGHT_IN_TILES = SCREEN_HEIGHT / TILE_SIZE;
    if (curSelect < 1)
        curSelect = 1;
    if (options < 0)
        return ANYWHERE_QUIT;
    cursor.drawRect = (SDL_Rect) {.x = TILE_SIZE, .y = (curSelect + 4) * TILE_SIZE, .w = TILE_SIZE, .h = TILE_SIZE};
    SDL_Event e;
    bool quit = false;
    int selection = -1;
    //While application is running
    while(!quit)
    {
        if (border)
            SDL_SetRenderDrawColor(mainRenderer, textColor.r, textColor.g, textColor.b, 0xFF);
        else
            SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);

        SDL_RenderClear(mainRenderer);
        SDL_RenderFillRect(mainRenderer, NULL);
        SDL_SetRenderDrawColor(mainRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
        SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = SCREEN_WIDTH / 128, .y = SCREEN_HEIGHT / 128, .w = 126 * SCREEN_WIDTH / 128, .h = 126 * SCREEN_HEIGHT / 128}));
        //background text (drawn first)
        drawText(title, TILE_SIZE + TILE_SIZE / 4, 11 * SCREEN_HEIGHT / 128, SCREEN_WIDTH, 119 * SCREEN_HEIGHT / 128, titleColorUnder, false);
        //foreground text
        drawText(title, TILE_SIZE + TILE_SIZE / 4, 5 * SCREEN_HEIGHT / 64, SCREEN_WIDTH, 55 * SCREEN_HEIGHT / 64, titleColorOver, false);

        for(int i = 0; ((options <= MAX_ITEMS) ? i < options : i < MAX_ITEMS); i++)
	        drawText(optionsArray[i], 2.25 * TILE_SIZE, (5 + i) * TILE_SIZE, SCREEN_WIDTH, (HEIGHT_IN_TILES - (5 + i)) * TILE_SIZE, textColor, false);
        if (extraDrawing)
            (*extraDrawing)();

        //SDL_RenderFillRect(mainRenderer, &((SDL_Rect){.x = cursor.x, .y = cursor.y, .w = cursor.w, .h = cursor.w}));
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
        drawCSprite(cursor, (cCamera) {(SDL_Rect) {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, 1.0, 0.0}, true, false);
    }
    return selection;
}
