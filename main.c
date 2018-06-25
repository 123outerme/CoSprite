#include "crankMain.h"
#include "crankFile.h"
#include "crankGraphics.h"
#include "crankUtility.h"

void loadSprite(cSprite* sprite, char* filePath, SDL_Rect rect, SDL_Rect clipRect, double scale, SDL_RendererFlip flip, double degrees, void* subclass, int priority);

int main(int argc, char* argv[])
{
    int code = initCrank("cb.bmp", "Crank Test", 960, 480, "Px437_ITT_BIOS_X.ttf", 24);
    cSprite sprites[2];
    cText txts[1];
    loadSprite(&sprites[0], "cb.bmp", (SDL_Rect) {.x = 50, .y = 50, .w = 150, .h = 120}, (SDL_Rect) {0, 0, 150, 120}, 1.0, SDL_FLIP_NONE, 0, NULL, 4);
    loadSprite(&sprites[1], "cb.bmp", (SDL_Rect) {.x = 0, .y = 0, .w = 150, .h = 120}, (SDL_Rect) {0, 0, 150, 120}, 1.0, SDL_FLIP_NONE, 0, NULL, 5);
    initCText(&txts[0], "Hello world!", (SDL_Rect) {150, 150, 300, 300}, (SDL_Color) {0, 0, 0, 0xFF}, (SDL_Color) {0xFF, 0, 0, 0x00}, 1);
    c2DModel model;
    initC2DModel(&model, sprites, 2, 0, 0, 150, 150, 1.0, SDL_FLIP_NONE, 0.0, NULL, 5);
    cCamera camera;
    initCCamera(&camera, (SDL_Rect) {0, 0, windowW / 48, windowH / 48}, 1.0);
    cScene scene;
    initCScene(&scene, (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF}, &camera, sprites, 0, &model, 1, (cResource*) NULL, 0, txts, 1);
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
        drawCScene(&scene, true);
    }
    destroyCScene(&scene);
    return code;
}

void loadSprite(cSprite* sprite, char* filePath, SDL_Rect rect, SDL_Rect clipRect, double scale, SDL_RendererFlip flip, double degrees, void* subclass, int priority)
{
    SDL_Texture* tempTexture;
    loadIMG(filePath, &tempTexture);
    initCSprite(sprite, tempTexture, 1, rect, clipRect, scale, flip, degrees, subclass, priority);
}
