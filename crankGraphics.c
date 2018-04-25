#include "crankGraphics.h"

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
 * \param drawPriority - 0 - not drawn. 1-5 - drawn. Lower number = drawn later
 */
void initCSprite(cSprite* sprite, SDL_Texture* texture, int id, SDL_Rect rect, double scale, SDL_RendererFlip flip, double degrees, void* subclass, int drawPriority)
{
    sprite->texture = texture;
    sprite->id = id;
    sprite->rect = rect;
    sprite->scale = scale;
    sprite->degrees = degrees;
    sprite->flip = flip;
    sprite->subclass = subclass;
    sprite->drawPriority = drawPriority;
}

/** \brief clears out a cSprite and its memory
 *
 * \param sprite - cSprite pointer
 */
void destroyCSprite(cSprite* sprite)
{
    SDL_DestroyTexture(sprite->texture);
    sprite->rect = (SDL_Rect) {0, 0, 0, 0};
    sprite->id = 0;
    sprite->scale = 0;
    sprite->degrees = 0;
    sprite->flip = SDL_FLIP_NONE;
    sprite->subclass = NULL;
    sprite->drawPriority = 0;

}

/** \brief draws a pSprite to the screen
 *
 * \param sprite - pSprite you want drawn
 * \param update - if true, immediately presents renderer
 */
void drawCSprite(cSprite sprite, bool update)
{
    SDL_RenderCopyEx(mainRenderer, sprite.texture, &((SDL_Rect) {.x = 0, .y = 0, .w = sprite.rect.w, .h = sprite.rect.h}), &((SDL_Rect) {.x = sprite.rect.x, .y = sprite.rect.y, .w = sprite.rect.w * sprite.scale, .h = sprite.rect.h * sprite.scale}), sprite.degrees, NULL, sprite.flip);
    if (update)
        SDL_RenderPresent(mainRenderer);
}

/** \brief initializes a cText object
 *
 * \param text - a pointer to your cText
 * \param string - the string you want
 * \param rect - SDL_Rect containing bounding box of text
 * \param textColor - color of text
 * \param bgColor - color of background box
 * \param drawPriority - 0 - not drawn. 1-5 - drawn. Lower number = drawn later
 */
void initCText(cText* text, char* string, SDL_Rect rect, SDL_Color textColor, SDL_Color bgColor, int drawPriority)
{
    text->string = calloc(strlen(string), sizeof(char));
    strcpy(text->string, string);
    text->rect = rect;
    text->textColor = textColor;
    text->bgColor = bgColor;
    text->drawPriority = drawPriority;
}

void destroyCText(cText* text)
{
    free(text->string);
    text->string = NULL;
    text->rect = (SDL_Rect) {0, 0, 0, 0};
    text->textColor = (SDL_Color) {0, 0, 0, 0};
    text->bgColor = (SDL_Color) {0, 0, 0, 0};
    text->drawPriority = 0;
}

void drawCText(cText text, bool update)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(mainRenderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(mainRenderer, text.bgColor.r, text.bgColor.g, text.bgColor.b, text.bgColor.a);
    SDL_RenderFillRect(mainRenderer, &(text.rect));
    drawText(text.string, text.rect.x, text.rect.y, text.rect.w, text.rect.h, text.textColor, update);
    SDL_SetRenderDrawColor(mainRenderer, r, g, b, a);
}

/** \brief Loads in an image resource
 *
 * \param res - cResource pointer
 * \param filepath - valid string filepath (relative or absolute)
 */
void initCResource(cResource* res, char* filepath)
{
    res->filepath = filepath;
    loadIMG(filepath, &(res->texture));
    SDL_QueryTexture(res->texture, NULL, NULL, &(res->w), &(res->h));
}

/** \brief clears out a cResource and its memory
 *
 * \param res - cResource pointer
 */
void destroyCResource(cResource* res)
{
    strcpy(res->filepath, "\0");
    SDL_DestroyTexture(res->texture);
    res->w = 0;
    res->h = 0;
}

/** \brief Initializes a cScene object.
 *
 * \param scenePtr - pointer to your cScene
 * \param sprites[] - array of pointers to cSprites to be drawn
 * \param spriteCount - how many elements in sprites[]
 * \param resources[] - array of pointers to cResources
 * \param resCount - how many elements in resources[]
 * \param strings[] - array of pointers to cTexts
 * \param stringCount - how many elements in strings[]
 */
void initCScene(cScene* scenePtr, SDL_Color bgColor, cSprite* sprites[], int spriteCount, cResource* resources[], int resCount, cText* strings[], int stringCount)
{
    scenePtr->bgColor = bgColor;
    scenePtr->sprites = (cSprite**) sprites;
    scenePtr->spriteCount = spriteCount;
    scenePtr->resources = (cResource**) resources;
    scenePtr->resCount = resCount;
    scenePtr->strings = (cText**) strings;
    scenePtr->stringCount = stringCount;
}

/** \brief clears out a cScene and all its memory, including sprites.
 *
 * \param scenePtr - pointer to your cScene
 */
void destroyCScene(cScene* scenePtr)
{
    for(int i = 0; i < scenePtr->spriteCount; i++)
        destroyCSprite(scenePtr->sprites[i]);

    scenePtr->spriteCount = 0;

    for(int i = 0; i < scenePtr->resCount; i++)
        destroyCResource(scenePtr->resources[i]);

    scenePtr->resCount = 0;

    for(int i = 0; i < scenePtr->stringCount; i++)
        destroyCText(scenePtr->strings[i]);

    scenePtr->stringCount = 0;
}

/** \brief draws the CScene.
 *
 * \param scenePtr - pointer to your cScene
 * \param redraw - if nonzero, will update the screen
 */
void drawCScene(cScene* scenePtr, bool redraw)
{
    SDL_SetRenderDrawColor(mainRenderer, scenePtr->bgColor.r, scenePtr->bgColor.g, scenePtr->bgColor.b, scenePtr->bgColor.a);
    SDL_RenderClear(mainRenderer);
    for(int priority = 5; priority >= 1; priority--)
    {
        for(int i = 0; i < scenePtr->spriteCount; i++)
        {
            if (scenePtr->sprites[i]->drawPriority == priority)
                drawCSprite(*(scenePtr->sprites[i]), false);
        }

        for(int i = 0; i < scenePtr->stringCount; i++)
        {
            if (scenePtr->strings[i]->drawPriority == priority)
                drawCText(*(scenePtr->strings[i]), false);
        }
    }

    if (redraw)
        SDL_RenderPresent(mainRenderer);
}

/** \brief Draws text to the screen using mainFont, wrapped and bounded
 *
 * \param input - text to be drawn
 * \param x - x value of first letter
 * \param y - y value of first letter
 * \param maxW - how wide the text can get before wrapping
 * \param maxH - how tall the text can draw before being cut off
 * \param color - SDL_Color struct of color to be used
 * \param render - if true, immediately presents renderer
 */
void drawText(char* input, int x, int y, int maxW, int maxH, SDL_Color color, bool render)
{
    if (canDrawText)
    {
        SDL_Texture* txtTexture = NULL;
        int* wh;
        wh = loadTextTexture(input, &txtTexture, maxW, color, true);
        SDL_RenderCopy(mainRenderer, txtTexture, &((SDL_Rect){.w = *wh > maxW ? maxW : *wh, .h = *(wh + 1) > maxH ? maxH : *(wh + 1)}),
                                                 &((SDL_Rect){.x =  x, .y = y, .w = *wh > maxW ? maxW : *wh, .h = *(wh + 1) > maxH ? maxH : *(wh + 1)}));


        if (render)
            SDL_RenderPresent(mainRenderer);
        SDL_DestroyTexture(txtTexture);
    }
}
