#include "crankMain.h"
#include "crankFile.h"
#include "crankGraphics.h"

void loadSprite(cSprite* sprite, char* filePath, SDL_Rect rect, double scale, SDL_RendererFlip flip, double degrees, void* subclass, int priority);

int main(int argc, char* argv[])
{
    int code = initCrank("cb.bmp", "Crank Test", 960, 480, "Px437_ITT_BIOS_X.ttf", 24);
    cSprite spr;
    cText txt;
    loadSprite(&spr, "cb.bmp", (SDL_Rect) {.x = 0, .y = 0, .w = 100, .h = 100}, 1.0, SDL_FLIP_NONE, 0, NULL, 5);
    initCText(&txt, "Hello world!", (SDL_Rect) {150, 150, 300, 300}, (SDL_Color) {0, 0, 0, 0xFF}, (SDL_Color) {0xFF, 0, 0, 0x00}, 1);
    cSprite* sprites[1] = {&spr};
    cText* txts[1] = {&txt};
    cScene scene;
    initCScene(&scene, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, sprites, 1, (cResource**) NULL, 0, txts, 1);
        printf("a\n");
    int key;
    while(key != -1)
    {
        key = getKey();
        if (key == SDLK_w)
            spr.rect.y -= 8;
        if (key == SDLK_s)
            spr.rect.y += 8;
        if (key == SDLK_a)
            spr.rect.x -= 8;
        if (key == SDLK_d)
            spr.rect.x += 8;
        drawCScene(&scene, true);
    }
    destroyCScene(&scene);
    return code;
}

void loadSprite(cSprite* sprite, char* filePath, SDL_Rect rect, double scale, SDL_RendererFlip flip, double degrees, void* subclass, int priority)
{
    SDL_Texture* tempTexture;
    loadIMG(filePath, &tempTexture);
    initCSprite(sprite, tempTexture, 1, rect, scale, flip, degrees, subclass, priority);
}
