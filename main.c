#include "crankMain.h"
#include "crankFile.h"

void loadSprite(cSprite* sprite, char* filePath, SDL_Rect rect, double scale, SDL_RendererFlip flip, double degrees, void* subclass);

int main(int argc, char* argv[])
{
    int code = initCrank("cb.bmp", "Crank Test", 960, 480, "Px437_ITT_BIOS_X.ttf", 24);
    cSprite cb;
    loadSprite(&cb, "cb.bmp", (SDL_Rect) {.x = 0, .y = 0, .w = 100, .h = 100}, 1.0, SDL_FLIP_NONE, 0, NULL);
    drawCSprite(cb, false);
    drawText("Hello world!", 150, 150, 300, 300, (SDL_Color) {0xFF, 0, 0, 0xFF}, true);
    waitForKey();
    replaceLine("README.md", 0, "# Crank", 256, true);
    return code;
}

void loadSprite(cSprite* sprite, char* filePath, SDL_Rect rect, double scale, SDL_RendererFlip flip, double degrees, void* subclass)
{
    SDL_Texture* tempTexture;
    loadIMG(filePath, &tempTexture);
    initCSprite(sprite, tempTexture, rect, scale, flip, degrees, subclass);
}
