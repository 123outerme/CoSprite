#include "csMain.h"
#include "csFile.h"
#include "csGraphics.h"
#include "csUtility.h"

void loadSprite(cSprite* sprite, char* filePath, SDL_Rect rect, SDL_Rect clipRect, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int priority);

int main(int argc, char* argv[])
{
    int code = initCrank("cb.bmp", "CoSprite Test/Example", 960, 480, "Px437_ITT_BIOS_X.ttf", 24);
    cSprite lowerSprite, upperSprite;
    cText txt;
    loadSprite(&lowerSprite, "cb.bmp", (SDL_Rect) {50, 50, 150, 120}, (SDL_Rect) {0, 0, 150, 120}, 1.0, SDL_FLIP_NONE, 0, false, NULL, 4);
    loadSprite(&upperSprite, "cb.bmp", (SDL_Rect) {0, 0, 150, 120}, (SDL_Rect) {0, 0, 150, 120}, 1.0, SDL_FLIP_NONE, 0, false, NULL, 5);
    initCText(&txt, "Hello world!", (SDL_Rect) {150, 150, 300, 300}, (SDL_Color) {0, 0, 0, 0xFF}, (SDL_Color) {0xFF, 0, 0, 0x00}, false, 1);
    c2DModel model;
    initC2DModel(&model, (cSprite[2]) {lowerSprite, upperSprite}, 2, 0, 0, 150, 150, 1.0, SDL_FLIP_NONE, 0.0, false, NULL, 5);
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
        drawCScene(&scene, true);
    }
    destroyCScene(&scene);
    return code;
}

void loadSprite(cSprite* sprite, char* filePath, SDL_Rect rect, SDL_Rect clipRect, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int priority)
{
    SDL_Texture* tempTexture;
    loadIMG(filePath, &tempTexture);
    initCSprite(sprite, tempTexture, 1, rect, clipRect, scale, flip, degrees, fixed, subclass, priority);
}
