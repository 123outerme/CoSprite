#include "pistonMain.h"
#include "pistonFile.h"

void loadSprite(pSprite* sprite, char* filePath, SDL_Rect rect, double scale, SDL_RendererFlip flip, double degrees, void* subclass);

int main(int argc, char* argv[])
{
    int code = initPiston("cb.bmp", "Piston Test", 960, 480, "", 24);
    pSprite cb;
    loadSprite(&cb, "cb.bmp", (SDL_Rect) {.x = 0, .y = 0, .w = 100, .h = 100}, 1.0, SDL_FLIP_NONE, 0, NULL);
    drawPSprite(cb, true);
    waitForKey();
    replaceLine("README.md", 2, "A new attempt at a 2D engine after outermeSDL really fell short in terms of usability.", 256);
    return code;
}

void loadSprite(pSprite* sprite, char* filePath, SDL_Rect rect, double scale, SDL_RendererFlip flip, double degrees, void* subclass)
{
    SDL_Texture* tempTexture;
    loadIMG(filePath, &tempTexture);
    initPSprite(sprite, tempTexture, rect, scale, flip, degrees, subclass);
}
