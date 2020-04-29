#include "csGraphics.h"

/** \brief Initializes a cSprite object. You may want to create a wrapper method.
 *
 * \param sprite - a pointer to your sprite.
 * \param texture - an SDL_Texture with your sprite's image
 * \param textureFilepath - a char* that holds your texture's filepath
 * \param x - x position onscreen
 * \param y - y position onscreen
 * \param w - width of your sprite
 * \param h - height of your sprite
 * \param scale - size * this == drawn size
 * \param flip - SDL_RenderFlip value
 * \param degrees - rotation angle in degrees
 * \param subclass - void*. Do with it what you will, isn't used internally
 * \param fixed - if true, won't be affected by a scene's camera
 * \param renderLayer - 0 - not drawn. 1-`renderLayers` - drawn. Lower number = drawn later
 */
void initCSprite(cSprite* sprite, SDL_Texture* texture, char* textureFilepath, int id, cDoubleRect drawRect, cDoubleRect srcClipRect, cDoublePt* center, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int renderLayer)
{
    sprite->texture = texture;
    strncpy(sprite->textureFilepath, textureFilepath, MAX_PATH);
    sprite->id = id;
    sprite->drawRect = drawRect;
    sprite->srcClipRect = srcClipRect;

    if (center != NULL)
        sprite->center = *center;
    else
        sprite->center = (cDoublePt) {drawRect.w / 2, drawRect.h / 2};

    sprite->scale = scale;
    sprite->degrees = degrees;
    sprite->flip = flip;
    sprite->fixed = fixed;
    sprite->subclass = subclass;
    sprite->renderLayer = renderLayer;
}

/** \brief clears out a cSprite and its memory
 *
 * \param sprite - cSprite pointer
 */
void destroyCSprite(cSprite* sprite)
{
    SDL_DestroyTexture(sprite->texture);
    sprite->drawRect = (cDoubleRect) {0, 0, 0, 0};
    sprite->srcClipRect = (cDoubleRect) {0, 0, 0, 0};
    sprite->id = 0;
    sprite->scale = 0;
    sprite->degrees = 0;
    sprite->flip = SDL_FLIP_NONE;
    sprite->fixed = false;
    free(sprite->subclass);
    sprite->subclass = NULL;
    sprite->renderLayer = 0;

}

/** \brief draws a cSprite to the screen
 *
 * \param sprite - cSprite you want drawn
 * \param camera - cCamera to be used for drawing
 * \param update - if true, immediately presents renderer
 * \param fixedOverride - if true, acts as if sprite.fixed is true
 */
void drawCSprite(cSprite sprite, cCamera camera, bool update, bool fixedOverride)
{
    double scale = sprite.scale * (sprite.fixed ? 1.0 : camera.zoom);
    cDoublePt point = (cDoublePt) {sprite.drawRect.x * scale, sprite.drawRect.y * scale};
    point = rotatePoint(point, (cDoublePt) {sprite.drawRect.x + sprite.center.x * scale, sprite.drawRect.y + sprite.center.y * scale}, sprite.degrees);
    if (!(sprite.fixed | fixedOverride))
    {
        point = rotatePoint(point, (cDoublePt) {global.windowW / 2, global.windowH / 2}, camera.degrees);
        point.x -= (camera.rect.x * global.windowW / camera.rect.w);
        point.y -= (camera.rect.y * global.windowH / camera.rect.h);
    }
    SDL_RenderCopyEx(global.mainRenderer, sprite.texture, &((SDL_Rect) {sprite.srcClipRect.x, sprite.srcClipRect.y, sprite.srcClipRect.w, sprite.srcClipRect.h}),
                     &((SDL_Rect) {.x = point.x /*- ((sprite.drawRect.w / 2) * sprite.scale * camera.zoom)*/, .y = point.y /*- ((sprite.drawRect.h / 2) * sprite.scale * camera.zoom)*/, .w = sprite.drawRect.w * sprite.scale * (sprite.fixed ? 1.0 : camera.zoom), .h = sprite.drawRect.h * sprite.scale * (sprite.fixed ? 1.0 : camera.zoom)}),
                     sprite.degrees + (!sprite.fixed * camera.degrees), &((SDL_Point) {0, 0}), sprite.flip);
    if (update)
        SDL_RenderPresent(global.mainRenderer);
}

/** \brief Initializes a c2DModel object.
 *
 * \param model - a pointer to your model.
 * \param sprites - a pointer that holds your sprites.
 * \param position - x/y of your model. Width and height will be filled in automatically.
 * \param center - a pointer to a cDoublePt that is the relative center. NULL to be (w/2, h/2).
 * \param scale - size * this == drawn size
 * \param flip - SDL_RenderFlip value
 * \param degrees - rotation angle in degrees
 * \param fixed - if true, won't be affected by a scene's camera
 * \param subclass - void*. Do with it what you will, isn't used internally
 * \param renderLayer - 0 - not drawn. 1-`renderLayers` - drawn. Lower number = drawn later
 */
void initC2DModel(c2DModel* model, cSprite* sprites, int numSprites, cDoublePt position, cDoublePt* center, double scale, SDL_RendererFlip flip, double degrees, bool fixed, void* subclass, int renderLayer)
{
    model->sprites = calloc(numSprites, sizeof(cSprite));
    memcpy((void*) model->sprites, (void*) sprites, numSprites * sizeof(cSprite));
    //model->sprites = (numSprites) ? sprites : NULL;
    model->numSprites = numSprites;
    model->rect = (cDoubleRect) {position.x, position.y, 0, 0};
    if (center != NULL)
        model->center = *center;
    else
        model->center = (cDoublePt) {model->rect.w / 2, model->rect.h / 2};
    model->scale = scale;
    model->flip = flip;
    model->degrees = degrees;
    model->fixed = fixed;
    model->subclass = subclass;
    model->renderLayer = renderLayer;
}

/** \brief clears out a c2DModel and its memory
 *
 * \param model - c2DModel pointer
 */
void destroyC2DModel(c2DModel* model)
{
    for(int i = 0; i < model->numSprites; i++)
        destroyCSprite(&model->sprites[i]);
    free(model->sprites);
    model->rect = (cDoubleRect) {0, 0, 0, 0};
    model->scale = 0;
    model->degrees = 0;
    model->flip = SDL_FLIP_NONE;
    model->fixed = false;
    free(model->subclass);
    model->subclass = NULL;
    model->renderLayer = 0;
}

/** \brief loads a C2DModel from a file
 *
 * \param model - c2DModel you want the data to be loaded to
 * \param filepath - where to get the file from
*/
void importC2DModel(c2DModel* model, char* filepath)
{
    char* data = calloc(2048, sizeof(char));
    readLine(filepath, 0, 2048, &data);
    model->numSprites = strtol(strtok(data, "{,}"), NULL, 10);
    model->rect.x = strtod(strtok(NULL, "{,}"), NULL);
    model->rect.y = strtod(strtok(NULL, "{,}"), NULL);
    model->rect.w = strtod(strtok(NULL, "{,}"), NULL);
    model->rect.h = strtod(strtok(NULL, "{,}"), NULL);
    model->center.x = strtod(strtok(NULL, "{,}"), NULL);
    model->center.y = strtod(strtok(NULL, "{,}"), NULL);
    model->scale = strtod(strtok(NULL, "{,}"), NULL);
    model->flip = strtol(strtok(NULL, "{,}"), NULL, 10);
    model->degrees = strtod(strtok(NULL, "{,}"), NULL);
    model->renderLayer = strtol(strtok(NULL, "{,}"), NULL, 10);
    model->fixed = strtol(strtok(NULL, "{,}"), NULL, 10);
    model->sprites = calloc(model->numSprites, sizeof(cSprite));
    for(int i = 0; i < model->numSprites; i++)
    {
        readLine(filepath, i + 1, 2048, &data);
        strncpy(model->sprites[i].textureFilepath, strtok(data, "{,}"), MAX_PATH);
        if (i == 0)
            loadIMG(model->sprites[i].textureFilepath, &(model->sprites[i].texture));
        else
        {
            model->sprites[i].texture = NULL;
            for(int x = i - 1; x >= 0; x--)
            {
                if (!strcmp(model->sprites[i].textureFilepath, model->sprites[x].textureFilepath))
                {
                    model->sprites[i].texture = model->sprites[x].texture;
                    break;
                }
            }
            if (!model->sprites[i].texture)
                loadIMG(model->sprites[i].textureFilepath, &(model->sprites[i].texture));
        }
        model->sprites[i].id = strtol(strtok(NULL, "{,}"), NULL, 10);
        model->sprites[i].drawRect.x = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].drawRect.y = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].drawRect.w = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].drawRect.h = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].srcClipRect.x = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].srcClipRect.y = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].srcClipRect.w = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].srcClipRect.h = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].center.x = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].center.y = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].scale = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].flip = strtol(strtok(NULL, "{,}"), NULL, 10);
        model->sprites[i].degrees = strtod(strtok(NULL, "{,}"), NULL);
        model->sprites[i].renderLayer = strtol(strtok(NULL, "{,}"), NULL, 10);
        model->sprites[i].fixed = strtol(strtok(NULL, "{,}"), NULL, 10);
        model->sprites[i].subclass = NULL;
    }
    free(data);
    model->subclass = NULL;
}

/** \brief converts a c2DModel into text and saves it to a file
 *
 * \param model - c2DModel you want saved
 * \param filepath - where to save the file to
*/
void exportC2DModel(c2DModel* model, char* filepath)
{
    createFile(filepath);
    char* data = calloc(2048, sizeof(char));
    snprintf(data, 2048, "{%d,%f,%f,%f,%f,%f,%f,%f,%d,%f,%d,%d}", model->numSprites, model->rect.x, model->rect.y, model->rect.w,
             model->rect.h, model->center.x, model->center.y, model->scale, model->flip, model->degrees,
             model->renderLayer, model->fixed);
    appendLine(filepath, data, true);
    for(int i = 0; i < model->numSprites; i++)
    {
        snprintf(data, 2048, "{%s,%d,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%d,%f,%d,%d}", model->sprites[i].textureFilepath, model->sprites[i].id,
                 model->sprites[i].drawRect.x, model->sprites[i].drawRect.y, model->sprites[i].drawRect.w, model->sprites[i].drawRect.h,
                 model->sprites[i].srcClipRect.x, model->sprites[i].srcClipRect.y, model->sprites[i].srcClipRect.w,
                 model->sprites[i].srcClipRect.h, model->sprites[i].center.x, model->sprites[i].center.y, model->sprites[i].scale,
                 model->sprites[i].flip, model->sprites[i].degrees, model->sprites[i].renderLayer, model->sprites[i].fixed);
        appendLine(filepath, data, true);
    }
    free(data);
}


//TODO: Sort by priority then draw
/** \brief draws a c2DModel to the screen
 *
 * \param model - c2DModel you want drawn
 * \param camera - cCamera to be used for drawing
 * \param update - if true, immediately presents renderer
 */
void drawC2DModel(c2DModel model, cCamera camera, bool update)
{
    for(int priority = 5; priority >= 1; priority--)
    {
        for(int i = 0; i < model.numSprites; i++)
        {
            if (model.sprites[i].renderLayer == priority)
            {
                {
                    double scale = model.scale * model.sprites[i].scale * (model.fixed | model.sprites[i].fixed ? 1.0 : camera.zoom);
                    cDoublePt point = {(model.sprites[i].drawRect.x + model.rect.x) * scale, (model.sprites[i].drawRect.y + model.rect.y) * scale};

                    point = rotatePoint(
                                rotatePoint(point, (cDoublePt) {point.x + model.sprites[i].center.x * scale, point.y + model.sprites[i].center.y * scale}, model.sprites[i].degrees),
                            (cDoublePt) {(model.rect.x + model.center.x) * scale, (model.rect.y + model.center.y) * scale}, model.degrees);

                    if (!(model.sprites[i].fixed | model.fixed))
                    {
                        point = rotatePoint(point, (cDoublePt) {global.windowW / 2 , global.windowH / 2}, camera.degrees);

                        point.x -= camera.rect.x * global.windowW / camera.rect.w;
                        point.y -= camera.rect.y * global.windowH / camera.rect.h;
                    }

                    SDL_RenderCopyEx(global.mainRenderer, model.sprites[i].texture, &((SDL_Rect) {model.sprites[i].srcClipRect.x, model.sprites[i].srcClipRect.y, model.sprites[i].srcClipRect.w, model.sprites[i].srcClipRect.h}),
                                     &((SDL_Rect) {.x = point.x /*- ((model.sprites[i].drawRect.w / 2) * model.scale * model.sprites[i].scale * camera.zoom)*/, .y = point.y /*- ((model.sprites[i].drawRect.h / 2) * model.scale * model.sprites[i].scale * camera.zoom)*/, .w = model.sprites[i].drawRect.w * model.scale * model.sprites[i].scale * (model.sprites[i].fixed ? 1.0 : camera.zoom), .h = model.sprites[i].drawRect.h * model.scale * model.sprites[i].scale * (model.sprites[i].fixed ? 1.0 : camera.zoom)}),
                                     model.sprites[i].degrees + model.degrees + (!model.sprites[i].fixed * camera.degrees),
                                     &((SDL_Point) {0, 0}), model.flip == model.sprites[i].flip ? SDL_FLIP_NONE : (model.sprites[i].flip + model.flip) % 4);
                    if (update)
                        SDL_RenderPresent(global.mainRenderer);
                }
                /*model.sprites[i].drawRect.x += model.rect.x;
                model.sprites[i].drawRect.y += model.rect.y;
                drawCSprite(model.sprites[i], camera, update, model.fixed);
                model.sprites[i].drawRect.x -= model.rect.x;
                model.sprites[i].drawRect.y -= model.rect.y;*/

            }
        }
    }
}

/** \brief initializes a cText object
 *
 * \param text - a pointer to your cText
 * \param string - the string you want
 * \param rect - cDoubleRect containing bounding box of text
 * \param textColor - color of text
 * \param bgColor - color of background box
 * \param degrees - rotation angle in degrees
 * \param flip - SDL_RenderFlip value
 * \param fixed - if true, won't be affected by a scene's camera
 * \param renderLayer - 0 - not drawn. 1-`renderLayers` - drawn. Lower number = drawn later
 */
void initCText(cText* text, char* str, cDoubleRect rect, SDL_Color textColor, SDL_Color bgColor, double scale, SDL_RendererFlip flip, double degrees, bool fixed, int renderLayer)
{
    text->str = calloc(strlen(str), sizeof(char));
    if (text->str)
        strcpy(text->str, str);
    text->rect = rect;
    text->textColor = textColor;
    text->bgColor = bgColor;
    text->scale = scale;
    text->flip = flip;
    text->degrees = degrees;
    text->fixed = fixed;
    text->renderLayer = renderLayer;

    //init text texture
    int* wh = loadTextTexture(text->str, &text->texture, text->rect.w, text->textColor, true);
    text->rect.w = wh[0];
    text->rect.h = wh[1];
}

/** \brief updates string in a cText obj
 *
 * \param text - cText to modify
 * \param str - new string for the cText to display
 */
void updateCText(cText* text, char* str)
{
    text->str = calloc(strlen(str), sizeof(char));
    if (text->str)
        strcpy(text->str, str);

    //init text texture
    int* wh = loadTextTexture(text->str, &text->texture, text->rect.w, text->textColor, true);
    text->rect.w = wh[0];
    text->rect.h = wh[1];
}

/** \brief clears out a cText and its memory
 *
 * \param text - cText pointer
 */
void destroyCText(cText* text)
{
    free(text->str);
    text->str = NULL;
    text->rect = (cDoubleRect) {0, 0, 0, 0};
    text->textColor = (SDL_Color) {0, 0, 0, 0};
    text->bgColor = (SDL_Color) {0, 0, 0, 0};
    text->scale = 1.0;
    text->fixed = false;
    text->renderLayer = 0;
    SDL_DestroyTexture(text->texture);
}

/** \brief draws a cText to the screen
 *
 * \param text - cText you want drawn
 * \param camera - cCamera to be used for drawing
 * \param update - if true, immediately presents renderer
 */
void drawCText(cText text, cCamera camera, bool update)
{
    Uint8 r, g, b, a;
    SDL_GetRenderDrawColor(global.mainRenderer, &r, &g, &b, &a);
    SDL_SetRenderDrawColor(global.mainRenderer, text.bgColor.r, text.bgColor.g, text.bgColor.b, text.bgColor.a);

    if (!text.fixed)
    {
        cDoublePt point = rotatePoint((cDoublePt) {text.rect.x, text.rect.y}, (cDoublePt) {global.windowW / 2 - text.rect.w / 2, global.windowH / 2 - text.rect.h / 2}, camera.degrees);

        text.rect.x = point.x - (camera.rect.x * global.windowW / camera.rect.w);
        text.rect.y = point.y - (camera.rect.y * global.windowH / camera.rect.h);
    }

    SDL_RenderCopyEx(global.mainRenderer, text.texture, NULL, &((SDL_Rect) {text.rect.x, text.rect.y, text.rect.w * text.scale, text.rect.h * text.scale}), text.degrees + !text.fixed * camera.degrees, NULL, text.flip);
    SDL_SetRenderDrawColor(global.mainRenderer, r, g, b, a);
    if (update)
        SDL_RenderPresent(global.mainRenderer);
}

/** \brief Loads in an image resource
 *
 * \param res - cResource pointer
 * \param subclass - struct containing your data
 * \param drawingMethod - function pointer to your drawing method. Must have only one argument, which is your subclass
 * \param drawingMethod - function pointer to your cleanup method. Must have only one argument, which is your subclass
 * \param renderLayer -
 */
void initCResource(cResource* res, void* subclass, void (*drawingRoutine)(void*, cCamera), void (*cleanupRoutine)(void*), int renderLayer)
{
    res->subclass = subclass;
    res->drawingRoutine = drawingRoutine;
    res->cleanupRoutine = cleanupRoutine;
    res->renderLayer = renderLayer;
}

/** \brief draws a CResource
 *
 * \param res - pointer to your cResource
 */
void drawCResource(cResource* res, cCamera camera)
{
    (*res->drawingRoutine)(res->subclass, camera);
}

/** \brief clears out a cResource and its memory
 *
 * \param res - cResource pointer
 */
void destroyCResource(cResource* res)
{
    if (res->cleanupRoutine != NULL)
        res->cleanupRoutine(res->subclass);

    res->subclass = NULL;
    res->drawingRoutine = NULL;
    res->cleanupRoutine = NULL;
    res->renderLayer = 0;
}

/** \brief initializes a cCamera and its memory
 *
 * \param camera - a cCamera pointer
 * \param rect - the bounding rect of the camera
 * \param degrees - angle of rotation in degrees
 */
void initCCamera(cCamera* camera, cDoubleRect rect, double zoom, double degrees)
{
    camera->rect = rect;
    camera->zoom = zoom;
    camera->degrees = degrees;
}

/** \brief clears out a cCamera and its memory
 *
 * \param camera - a cCamera pointer
 */
void destroyCCamera(cCamera* camera)
{
    camera->rect = (cDoubleRect) {0, 0, 0, 0};
    camera->zoom = 1.0;
    camera->degrees = 0.0;
}

/** \brief Initializes a cScene object.
 *
 * \param scenePtr - pointer to your cScene
 * \param sprites[] - array of cSprites to be drawn
 * \param spriteCount - how many elements in sprites[]
 * \param models[] - array of c2DModels to be drawn
 * \param modelCount - how many elements in model[]
 * \param resources[] - array of cResources
 * \param resCount - how many elements in resources[]
 * \param strings[] - array of cTexts
 * \param stringCount - how many elements in strings[]
 */
void initCScene(cScene* scenePtr, SDL_Color bgColor, cCamera* camera, cSprite* sprites[], int spriteCount, c2DModel* models[], int modelCount, cResource* resources[], int resCount, cText* strings[], int stringCount)
{
    scenePtr->camera = camera;
    scenePtr->bgColor = bgColor;
    if (spriteCount > 0)
    {
        scenePtr->sprites = calloc(spriteCount, sizeof(cSprite*));
        for(int i = 0; i < spriteCount; i++)
            scenePtr->sprites[i] = sprites[i];
    }
    else
        scenePtr->sprites = NULL;
    scenePtr->spriteCount = spriteCount;

    if (modelCount > 0)
    {
        scenePtr->models = calloc(modelCount, sizeof(c2DModel*));
        for(int i = 0; i < modelCount; i++)
            scenePtr->models[i] = models[i];
    }
    else
        scenePtr->models = NULL;
    scenePtr->modelCount = modelCount;

    if (resCount > 0)
    {
        scenePtr->resources = calloc(resCount, sizeof(cResource*));
        for(int i = 0; i < resCount; i++)
            scenePtr->resources[i] = resources[i];
    }
    else
        scenePtr->resources = NULL;
    scenePtr->resCount = resCount;

    if (stringCount > 0)
    {
        scenePtr->strings = calloc(stringCount, sizeof(cText*));
        for(int i = 0; i < stringCount; i++)
            scenePtr->strings[i] = strings[i];
    }
    else
        scenePtr->strings = NULL;
    scenePtr->stringCount = stringCount;
}

/** \brief Adds a cSprite to a scene.
 *
 * \param scenePtr - pointer to a cScene
 * \param sprite - pointer to a sprite
 * \return true if succeeded, false if not
 */
int addSpriteToCScene(cScene* scenePtr, cSprite* sprite)
{
    bool success = true;
    cSprite** tempSprites;
    if (scenePtr->modelCount == 0)
        tempSprites = calloc(scenePtr->spriteCount + 1, sizeof(cSprite*));
    else
        tempSprites = realloc(scenePtr->sprites, (scenePtr->spriteCount + 1) * sizeof(cSprite*));

    if (tempSprites != NULL)
    {
        free(scenePtr->sprites);
        scenePtr->sprites = tempSprites;
        scenePtr->sprites[scenePtr->spriteCount] = sprite;
        scenePtr->spriteCount++;
    }
    else
    {
        printf("addSpriteToCScene: Failed to resize sprites array (attempted size %d)", scenePtr->spriteCount + 1);
        success = false;
    }
    return success;
}

//fix the rest of the add methods

/** \brief Removes a specified sprite, or the sprite in the given index from a scene.
 *
 * \param scenePtr - pointer to a cScene
 * \param sprite - pointer to the sprite to match addresses (NULL to use index instead)
 * \param index - where in the sprite array the sprite is (-1 to make sure it is not used)
 * \param free - if true, cleans up the referenced sprite from memory
 * \return true if succeeded, false if not
 */
int removeSpriteFromCScene(cScene* scenePtr, cSprite* sprite, int index, bool free)
{
    bool success = true;
    if (sprite != NULL)
    {
        index = -1;
        for(int i = 0; i < scenePtr->spriteCount; i++)
        {
            if (scenePtr->sprites[i] == sprite)
                index = i;
        }
    }
    if (free)
        destroyCSprite(scenePtr->sprites[index]);

    if (index >= 0)
    {
        scenePtr->sprites[index] = NULL;
        for(int i = index; i < scenePtr->spriteCount - 1; i++)
            scenePtr->sprites[i] = scenePtr->sprites[i + 1];

        scenePtr->spriteCount--;
    }
    else
    {
        printf("removeSpriteFromCScene: Failed to find sprite\n");
        success = false;
    }
    return success;
}

/** \brief Adds a c2DModel to a scene.
 *
 * \param scenePtr - pointer to a cScene
 * \param model - pointer to a c2DModel
 * \return true if succeeded, false if not
 */
int add2DModelToCScene(cScene* scenePtr, c2DModel* model)
{
    bool success = true;
    c2DModel** tempModels;
    if (scenePtr->modelCount == 0)
        tempModels = calloc(scenePtr->modelCount + 1, sizeof(c2DModel*));
    else
        tempModels = realloc(scenePtr->models, (scenePtr->modelCount + 1) * sizeof(c2DModel*));

    if (tempModels != NULL)
    {

        free(scenePtr->models);
        scenePtr->models = tempModels;
        scenePtr->models[scenePtr->modelCount] = model;
        scenePtr->modelCount++;
    }
    else
    {
        printf("add2DModelToCScene: Failed to resize models array (attempted size %d)", scenePtr->modelCount + 1);
        success = false;
    }
    return success;
}

/** \brief Removes a specified model, or the model in the given index from a scene.
 *
 * \param scenePtr - pointer to a cScene
 * \param model - pointer to the model to match addresses (NULL to use index instead)
 * \param index - where in the sprite array the sprite is (-1 to make sure it is not used)
 * \param free - if true, cleans up the referenced model from memory
 * \return true if succeeded, false if not
 */
int remove2DModelFromCScene(cScene* scenePtr, c2DModel* model, int index, bool free)
{
    bool success = true;
    if (model != NULL)
    {
        index = -1;
        for(int i = 0; i < scenePtr->modelCount; i++)
        {
            if (scenePtr->models[i] == model)
                index = i;
        }
    }
    if (free)
        destroyC2DModel(scenePtr->models[index]);

    if (index >= 0)
    {
        scenePtr->models[index] = NULL;
        for(int i = index; i < scenePtr->modelCount - 1; i++)
            scenePtr->models[i] = scenePtr->models[i + 1];

        scenePtr->modelCount--;
    }
    else
    {
        printf("remove2DModelFromCScene: Failed to find model\n");
        success = false;
    }
    return success;
}

/** \brief Adds a cText to a specified scene.
 *
 * \param scenePtr - pointer to a cScene
 * \param text- pointer to a cText
 * \return true if succeeded, false if not
 */
int addTextToCScene(cScene* scenePtr, cText* text)
{
    bool success = true;
    cText** tempTexts;
    if (scenePtr->stringCount == 0)
        tempTexts = calloc(scenePtr->stringCount + 1, sizeof(cText*));
    else
        tempTexts = realloc(scenePtr->strings, scenePtr->stringCount * sizeof(cText*));

    if (tempTexts != NULL)
    {
        free(scenePtr->strings);
        scenePtr->strings = tempTexts;
        scenePtr->strings[scenePtr->modelCount] = text;
        scenePtr->stringCount++;
    }
    else
    {
        printf("addTextToCScene: Failed to resize strings array (attempted size %d)", scenePtr->stringCount + 1);
        success = false;
    }
    return success;
}

/** \brief Removes a text, or the text in a given index from a scene.
 *
 * \param scenePtr - pointer to a cScene
 * \param text - pointer to the text to match addresses (NULL to use index instead)
 * \param index - where in the sprite array the sprite is (-1 to make sure it is not used)
 * \param free - if true, cleans up the referenced text from memory
 * \return true if succeeded, false if not
 */
int removeTextFromCScene(cScene* scenePtr, cText* text, int index, bool free)
{
    bool success = true;
    if (text != NULL)
    {
        index = -1;
        for(int i = 0; i < scenePtr->stringCount; i++)
        {
            if (scenePtr->strings[i] == text)
                index = i;
        }
    }
    if (free)
        destroyCText(scenePtr->strings[index]);

    if (index >= 0)
    {
        scenePtr->strings[index] = NULL;
        for(int i = index; i < scenePtr->stringCount - 1; i++)
                scenePtr->strings[i] = scenePtr->strings[i + 1];

        scenePtr->stringCount--;
    }
    else
    {
        printf("removeTextFromCScene: Failed to find text\n");
        success = false;
    }

    return success;
}

/** \brief Adds a cResource to the specified scene.
 *
 * \param scenePtr - pointer to a cScene
 * \param resource - pointer to a cResource
 * \return true if succeeded, false if not
 */
int addResourceToCScene(cScene* scenePtr, cResource* resource)
{
    bool success = true;
    cResource** tempResources;
    if (scenePtr->resCount == 0)
        tempResources = calloc(scenePtr->resCount + 1, sizeof(cResource*));
    else
        tempResources = realloc(scenePtr->resources, scenePtr->resCount * sizeof(cResource*));
    if (tempResources != NULL)
    {
        free(scenePtr->resources);
        scenePtr->resources = tempResources;
        scenePtr->resCount++;
    }
    else
    {
        printf("addresourceToCScene: Failed to resize resources array (attempted size %d)", scenePtr->resCount--);
        success = false;
    }
    return success;
}

/** \brief Removes a resource, or the resource in a given index from a scene.
 *
 * \param scenePtr - pointer to a cScene
 * \param resource - pointer to the resource to match addresses (NULL to use index instead)
 * \param index - where in the sprite array the sprite is (-1 to make sure it is not used)
 * \param free - if true, cleans up the referenced resource from memory
 * \return true if succeeded, false if not
 */
int removeResourceFromCScene(cScene* scenePtr, cResource* resource, int index, bool free)
{
    bool success = true;
    if (resource != NULL)
    {
        index = -1;
        for(int i = 0; i < scenePtr->resCount; i++)
        {
            if (scenePtr->resources[i] == resource)
                index = i;
        }
    }
    if (free)
        destroyCResource(scenePtr->resources[index]);

    if (index >= 0)
    {
        scenePtr->resources[index] = NULL;
        for(int i = index; i < scenePtr->resCount - 1; i++)
            scenePtr->resources[i] = scenePtr->resources[i + 1];

        scenePtr->resCount--;
    }
    else
    {
        printf("removeResourceFromCScene: Failed to find resource\n");
        success = false;
    }
    return success;
}

/** \brief clears out a cScene and all its memory, including sprites.
 *
 * \param scenePtr - pointer to your cScene
 */
void destroyCScene(cScene* scenePtr)
{
    destroyCCamera(scenePtr->camera);

    if (scenePtr->spriteCount > 0)
    {
        for(int i = 0; i < scenePtr->spriteCount; i++)
            destroyCSprite(scenePtr->sprites[i]);
        scenePtr->spriteCount = 0;
        free(scenePtr->sprites);
    }

    if (scenePtr->modelCount > 0)
    {
        for(int i = 0; i < scenePtr->modelCount; i++)
            destroyC2DModel(scenePtr->models[i]);
        scenePtr->modelCount = 0;
        free(scenePtr->models);
    }

    if (scenePtr->resCount > 0)
    {
        for(int i = 0; i < scenePtr->resCount; i++)
            destroyCResource(scenePtr->resources[i]);
        scenePtr->resCount = 0;
        free(scenePtr->resources);
    }

    if (scenePtr->stringCount > 0)
    {
        for(int i = 0; i < scenePtr->stringCount; i++)
            destroyCText(scenePtr->strings[i]);
        scenePtr->stringCount = 0;
        free(scenePtr->strings);
    }

    if (scenePtr->camera)
        destroyCCamera(scenePtr->camera);
}

/** \brief draws the CScene.
 *
 * \param scenePtr - pointer to your cScene
 * \param redraw - if nonzero, will update the screen
 */
void drawCScene(cScene* scenePtr, bool clearScreen, bool redraw)
{ //TODO: Speed this up
    SDL_SetRenderDrawColor(global.mainRenderer, scenePtr->bgColor.r, scenePtr->bgColor.g, scenePtr->bgColor.b, scenePtr->bgColor.a);
    if (clearScreen)
        SDL_RenderClear(global.mainRenderer);

    int maxNum = fmax(scenePtr->spriteCount, fmax(scenePtr->modelCount, fmax(scenePtr->resCount, scenePtr->stringCount)));

    for(int priority = global.renderLayers; priority >= 1; priority--)
    {
        /*for(int i = 0; i < scenePtr->spriteCount; i++)
        {
            if (scenePtr->sprites[i]->renderLayer == priority)
                drawCSprite(*(scenePtr->sprites[i]), *(scenePtr->camera), false, false);
        }

        for(int i = 0; i < scenePtr->modelCount; i++)
        {
            if (scenePtr->models[i]->renderLayer == priority)
                drawC2DModel(*(scenePtr->models[i]), *(scenePtr->camera), false);
        }

        for(int i = 0; i < scenePtr->resCount; i++)
        {
            if (scenePtr->resources[i]->renderLayer == priority)
                drawCResource(scenePtr->resources[i]);
        }

        for(int i = 0; i < scenePtr->stringCount; i++)
        {
            if (scenePtr->strings[i]->renderLayer == priority)
                drawCText(*(scenePtr->strings[i]), *(scenePtr->camera), false);
        }*/
        for(int i = 0; i < maxNum; i++)
        {
            if (scenePtr->spriteCount > i && scenePtr->sprites[i]->renderLayer == priority)
                drawCSprite(*(scenePtr->sprites[i]), *(scenePtr->camera), false, false);

            if (scenePtr->modelCount > i && scenePtr->models[i]->renderLayer == priority)
                drawC2DModel(*(scenePtr->models[i]), *(scenePtr->camera), false);

            if (scenePtr->resCount > i && scenePtr->resources[i]->renderLayer == priority)
                drawCResource(scenePtr->resources[i], *(scenePtr->camera));

            if (scenePtr->stringCount > i && scenePtr->strings[i]->renderLayer == priority)
                drawCText(*(scenePtr->strings[i]), *(scenePtr->camera), false);
        }
    }

    if (redraw)
        SDL_RenderPresent(global.mainRenderer);
}

/** \brief Draws text to the screen using `global.mainFont`, wrapped and bounded
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
    if (global.canDrawText)
    {
        SDL_Texture* txtTexture = NULL;
        int* wh;
        wh = loadTextTexture(input, &txtTexture, maxW, color, true);
        SDL_RenderCopy(global.mainRenderer, txtTexture, &((SDL_Rect){.w = *wh > maxW ? maxW : *wh, .h = *(wh + 1) > maxH ? maxH : *(wh + 1)}),
                            &((SDL_Rect){.x =  x, .y = y, .w = *wh > maxW ? maxW : *wh, .h = *(wh + 1) > maxH ? maxH : *(wh + 1)}));

        if (render)
            SDL_RenderPresent(global.mainRenderer);
        SDL_DestroyTexture(txtTexture);
    }
}

/** \brief uses SAT to check whether one sprite is colliding onscreen with another
 * \param sprite1 - The first cSprite
 * \param sprite2 - The second cSprite
 * \return Minimum translation vector for the collision (mag 0 if no collision)
*/
cDoubleVector checkCSpriteCollision(cSprite sprite1, cSprite sprite2)  //using the Separation Axis Theorem
{
    cDoublePt center1 = (cDoublePt) {(sprite1.center.x + sprite1.drawRect.x) * sprite1.scale, (sprite1.center.y + sprite1.drawRect.y) * sprite1.scale};

    cDoublePt corners1[4] = {rotatePoint((cDoublePt) {sprite1.drawRect.x * sprite1.scale, sprite1.drawRect.y * sprite1.scale}, center1, sprite1.degrees),
    rotatePoint((cDoublePt) {(sprite1.drawRect.x + sprite1.drawRect.w) * sprite1.scale, sprite1.drawRect.y * sprite1.scale}, center1, sprite1.degrees),
    rotatePoint((cDoublePt) {(sprite1.drawRect.x + sprite1.drawRect.w * sprite1.scale), (sprite1.drawRect.y + sprite1.drawRect.h) * sprite1.scale}, center1, sprite1.degrees),
    rotatePoint((cDoublePt) {sprite1.drawRect.x * sprite1.scale, (sprite1.drawRect.y + sprite1.drawRect.h * sprite1.scale)}, center1, sprite1.degrees)};
    //we find the corners of the first sprite, taking into account scale, center of rotation, and angle

    cDoublePt center2 = (cDoublePt) {(sprite2.center.x + sprite2.drawRect.x) * sprite2.scale, (sprite2.center.y + sprite2.drawRect.y) * sprite2.scale};

    cDoublePt corners2[4] = {rotatePoint((cDoublePt) {sprite2.drawRect.x * sprite2.scale, sprite2.drawRect.y * sprite2.scale}, center2, sprite2.degrees),
    rotatePoint((cDoublePt) {(sprite2.drawRect.x + sprite2.drawRect.w) * sprite2.scale, sprite2.drawRect.y * sprite2.scale}, center2, sprite2.degrees),
    rotatePoint((cDoublePt) {(sprite2.drawRect.x + sprite2.drawRect.w * sprite2.scale), (sprite2.drawRect.y + sprite2.drawRect.h) * sprite2.scale}, center2, sprite2.degrees),
    rotatePoint((cDoublePt) {sprite2.drawRect.x * sprite2.scale, (sprite2.drawRect.y + sprite2.drawRect.h * sprite2.scale)}, center2, sprite2.degrees)};
    //and same for the second sprite

    //cDoublePt realCenter1 = {(corners1[0].x + sprite1.drawRect.w / 2) * sprite1.scale, (corners1[0].y + sprite1.drawRect.h / 2) * sprite1.scale};
    //cDoublePt realCenter2 = {(corners2[0].x + sprite2.drawRect.w / 2) * sprite2.scale, (corners2[0].y + sprite2.drawRect.h / 2) * sprite2.scale};
    //get their physical centers, not their draw rotation centers

    /*SDL_SetRenderDrawColor(global.mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(global.mainRenderer);
    SDL_SetRenderDrawColor(global.mainRenderer, 0xFF, 0x00, 0x00, 0xFF);

    for(int i = 0; i < 4; i++)
        SDL_RenderDrawPoint(global.mainRenderer, corners1[i].x, corners1[i].y);  //debugging corner generation

    for(int i = 0; i < 4; i++)
        SDL_RenderDrawPoint(global.mainRenderer, corners2[i].x, corners2[i].y);

    SDL_RenderPresent(global.mainRenderer);
    waitForKey(true);*/

    double normals[8] = {sprite1.degrees, sprite1.degrees + 90, sprite2.degrees, sprite2.degrees + 90};
    //since we know we're dealing with rectangles, the normals can just be the angle each sprite is at and the angle + 90 degrees
    cDoubleVector minTranslationVector = (cDoubleVector) {-1, 0};

    /*int firstPts[2] = {0, 0};  //debugging found points
    int secondPts[2] = {0, 0};*/
    //int debugI = 0;
    for(int i = 0; i < 4; i++)
    {
        double min1 = sqrt(pow(corners1[0].x, 2) + pow(corners1[0].y, 2)) * fabs(cos(fabs(degToRad(normals[i]) - atan2(corners1[0].y, corners1[0].x))));
        double max1 = min1;
        for(int x = 1; x < 4; x++)
        {
            //sqrt(x^2 + y^2) * cos(theta);  //this is the projection of the selected corner onto a plane with some angle `theta` between them
            //essentially we get the magnitude of a vector from 0,0 to x,y (call it V) then multiply by the cosine of (the normal's angle - the angle of V)
            double newVal = sqrt(pow(corners1[x].x, 2) + pow(corners1[x].y, 2)) * fabs(cos(fabs(degToRad(normals[i]) - atan2(corners1[x].y, corners1[x].x))));
            if (newVal > max1)
            {
                max1 = newVal;
                //firstPts[1] = x;
            }
            if (newVal < min1)
            {
                min1 = newVal;
                //firstPts[0] = x;
            }
            //and we need to determine the minimum (closest to axis origin) and maximum (farther from axis origin)
            //by looping through each point, finding the projected point

        }

        double min2 = sqrt(pow(corners2[0].x, 2) + pow(corners2[0].y, 2)) * fabs(cos(fabs(degToRad(normals[i]) - atan2(corners2[0].y, corners2[0].x))));
        double max2 = min2;
        for(int x = 1; x < 4; x++)
        {
            double newVal = sqrt(pow(corners2[x].x, 2) + pow(corners2[x].y, 2)) * fabs(cos(fabs(degToRad(normals[i]) - atan2(corners2[x].y, corners2[x].x))));
            if (newVal > max2)
            {
                max2 = newVal;
                //secondPts[1] = x;
            }
            if (newVal < min2)
            {
                min2 = newVal;
                //secondPts[0] = x;
            }
        }
        //and the same thing for the second object as well

        /*printf("%.2f degrees\n%d, %d\n%d, %d\n\n%f, %f\n%f, %f\n\n", normals[i], firstPts[0], firstPts[1], secondPts[0], secondPts[1], min1, max1, min2, max2);

        SDL_RenderFillRect(global.mainRenderer, &((SDL_Rect) {.x = min1, .y = 0, .w = max1 - min1, .h = 4}));
        SDL_RenderFillRect(global.mainRenderer, &((SDL_Rect) {.x = min2, .y = 6, .w = max2 - min2, .h = 4}));
        SDL_RenderPresent(global.mainRenderer);  //debugging projection
        waitForKey(true);*/

        //printf("%f or %f\n", min2 - max1, min1 - max2);
        if (min2 >= max1 || min1 >= max2)
        {
            minTranslationVector = (cDoubleVector) {0, 0};
            break;
        }
        else
        {  //finding the overlap is glitched with rotated stuff when s1 pos closer to origin than s2, but almost fixed
            double overlap = max1 - min2, o2 = max2 - min1, degrees = normals[i];
            if (o2 < overlap)
                overlap = o2;
            if (min1 < min2)
                degrees += 180;  //somewhere around here is probably where MVT with rotated shapes is glitched

            if (fabs(overlap) < minTranslationVector.magnitude || minTranslationVector.magnitude == -1)
            {
                minTranslationVector = (cDoubleVector) {overlap, degrees};
                //debugI = i;
            }
        }
        //check for intersections of the two projected lines
        //  if not found, return false (because according to SAT if one gap in projections is found, there's a separating axis there)
        //  else continue
    }
    /*
    if (minTranslationVector.magnitude)
    {  //debugging MTV
        SDL_SetRenderDrawColor(global.mainRenderer, 0xFF * (normals[debugI] == minTranslationVector.degrees), 0x00, 0xFF * (normals[debugI] != minTranslationVector.degrees), 0xFF);
        SDL_RenderDrawLine(global.mainRenderer, corners1[0].x, corners1[0].y, corners1[0].x + minTranslationVector.magnitude * cos(degToRad(minTranslationVector.degrees)), corners1[0].y + minTranslationVector.magnitude * sin(degToRad(minTranslationVector.degrees)));
        SDL_SetRenderDrawColor(global.mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderPresent(global.mainRenderer);
    }//*/
    return minTranslationVector;
}

/** \brief checks whether a model's general hitbox collides with another (fast)
 * \param model1 - The first c2DModel
 * \param model2 - The second c2DModel
 * \param fast - if false checks each sprite of each model, else checks models' general hitbox
 * \return Minimum translation vector (mag 0 if not colliding)
*/
cDoubleVector checkC2DModelCollision(c2DModel model1, c2DModel model2, bool fast)
{
    if (fast)
    {
        cSprite sprite1;
        initCSprite(&sprite1, NULL, ".", 0, (cDoubleRect) {model1.rect.x * model1.scale, model1.rect.y * model1.scale, model1.rect.w * model1.scale, model1.rect.h * model1.scale}, model1.rect, &model1.center, model1.scale, model1.flip, model1.degrees, false, NULL, 0);
        cSprite sprite2;
        initCSprite(&sprite2, NULL, ".", 0, (cDoubleRect) {model2.rect.x * model2.scale, model2.rect.y * model2.scale, model2.rect.w * model2.scale, model2.rect.h * model2.scale}, model2.rect, &model2.center, model2.scale, model2.flip, model2.degrees, false, NULL, 0);
        return checkCSpriteCollision(sprite1, sprite2);
    }
    else
    {
        cDoubleVector mtv = (cDoubleVector) {-1, 0};
        for(int i = 0; i < model1.numSprites; i++)
        {
            for(int x = 0; x < model2.numSprites; x++)
            {
                cDoubleVector collisionV;
                collisionV = checkCSpriteCollision(model1.sprites[i], model2.sprites[x]);
                if (collisionV.magnitude < mtv.magnitude || mtv.magnitude == -1)
                    mtv = collisionV;
            }
        }
        return mtv;
    }
}

//
#define IMG_INIT_FLAGS IMG_INIT_PNG

/** \brief Initializes an SDL window and all of CoSprite's inner stuff.
 *
 * \return Code 0: No error. Code 1: SDL systems failed to initialize. Code 2: Window could not be created Code 3: Renderer failed to initialize
 */
int initCoSprite(char* iconPath, char* windowName, int windowWidth, int windowHeight, char* fontPath, int fontSize, int renderLayers, SDL_Color transparentColor, Uint32 windowFlags)
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
            status = 1;  //this isn't a fatal error
        }
        else
        {
            //Mix_Init(MIX_INIT_OGG);  //deprecated?
            global.soundVolume = MIX_MAX_VOLUME;
            Mix_AllocateChannels(32);
            Mix_Volume(-1, global.soundVolume);  //sets all channels to the sound level global.soundVolume
            global.musicVolume = MIX_MAX_VOLUME;
            Mix_VolumeMusic(global.musicVolume);
        }
        global.mainRenderer = NULL;
        global.mainFont = NULL;
        mainWindow = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, windowFlags);
        if (!mainWindow)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            return 2;
        }
        else
        {
            global.windows = calloc(1, sizeof(SDL_Window*));
            global.windows[0] = mainWindow;
            global.windowsOpen = 1;
            global.windowW = windowWidth;
            global.windowH = windowHeight;
            global.renderLayers = (renderLayers < 1) ? 6 : renderLayers;
            global.mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
            if(!global.mainRenderer)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                return 3;
            }
            else
            {
                global.colorKey = transparentColor;

                if (iconPath != NULL)
                {
                    SDL_Surface* iconSurface = IMG_Load(iconPath);
                    SDL_SetWindowIcon(mainWindow, iconSurface);
                    SDL_FreeSurface(iconSurface);
                }

                SDL_SetRenderDrawBlendMode(global.mainRenderer, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(global.mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderSetLogicalSize(global.mainRenderer, windowWidth, windowHeight);
                SDL_RenderClear(global.mainRenderer);
                loadTTFont(fontPath, &global.mainFont, fontSize);
                //loadTTFont(FONT_FILE_NAME, &smallFont, 20);
                if (!global.mainFont)
                {
                    global.canDrawText = false;
                    status = 4;
                }
                global.canDrawText = true;
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

/** \brief Uninitializes CoSprite. Usually you run this just before the program exits.
*/
void closeCoSprite()
{
    TTF_CloseFont(global.mainFont);
    //TTF_CloseFont(smallFont);
    if (mainWindow)
        SDL_DestroyWindow(mainWindow);
    if (global.mainRenderer)
        SDL_DestroyRenderer(global.mainRenderer);
    /*for(int i = 0; i < MAX_SOUNDS; i++)
    {
        if (audioArray[i])
            Mix_FreeChunk(audioArray[i]);
    }*/

    /*for(int i = 0; i < MAX_MUSIC; i++)
    {
        if (musicArray[i])
            Mix_FreeMusic(musicArray[i]);
    }*/

    TTF_Quit();
    IMG_Quit();
    Mix_CloseAudio();
    SDL_Quit();
}

/** \brief Opens another SDL2 Window and saves its information to the coSprite object `global`.
 *
 * \param windowPtr - the SDL_Window object you define for this window.
 * \param windowName - a string containing the name you want the window to have.
 * \param windowWidth - how wide the window should be, in pixels
 * \param window Height - how tall the window should be, in pixels
 * \return the position of the window in `global.windows[]`
*/
int openCWindow(SDL_Window* windowPtr, char* windowName, int windowWidth, int windowHeight)
{
    windowPtr = SDL_CreateWindow(windowName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    global.windows = realloc((void*) global.windows, global.windowsOpen + 1);
    global.windows[global.windowsOpen] = windowPtr;
    return global.windowsOpen++;
}

/** \brief closes a SDL2 window you opened with CoSprite.
 *
 * \param windowPos - the position of the window you want closed in `global.windows[]`. 0 is always the main window
*/
void closeCWindow(int windowPos)
{
    SDL_DestroyWindow(global.windows[windowPos]);
    global.windows[windowPos] = NULL;
    SDL_Window* theseWindows[global.windowsOpen];
    if (windowPos < global.windowsOpen - 1)
    {
        int k = 0;
        for(int i = 0; i < global.windowsOpen; i++)
        {
            if (global.windows[i] != NULL)
                theseWindows[k++] = global.windows[i];
        }
        memcpy(global.windows, theseWindows, k * sizeof(SDL_Window*));
    }
    global.windows = realloc((void*) global.windows, global.windowsOpen - 1);
    global.windowsOpen--;
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
    SDL_SetColorKey(surf, 1, SDL_MapRGB(surf->format, global.colorKey.r, global.colorKey.g, global.colorKey.b));
    *dest = SDL_CreateTextureFromSurface(global.mainRenderer, surf);
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

/** \brief converts text to a texture
 *
 * \param text - text you want converted to a texture
 * \param dest - pointer to your SDL_Texture*
 * \param maxW - How wide the text can be before wrapping
 * \param color - SDL_Color struct of color to be used
 * \param isBlended - true always
 * \return int[2] holding {width, height}
 *
 */
int* loadTextTexture(char* text, SDL_Texture** dest, int maxW, SDL_Color color, bool isBlended)
{
    static int wh[] = {0, 0};
    SDL_Surface* txtSurface = NULL;
    if (isBlended)
        txtSurface = TTF_RenderText_Blended_Wrapped(global.mainFont, text, color, maxW);
//    else
//        txtSurface = TTF_RenderText(smallFont, text, color, ((SDL_Color) {181, 182, 173}));
    *dest = SDL_CreateTextureFromSurface(global.mainRenderer, txtSurface);
    if (!*dest)
    {
        printf("Text texture could not be loaded! SDL Error: %s\n", SDL_GetError());
    }
    else
    {
        wh[0] = txtSurface->w;
        wh[1] = txtSurface->h;
    }
    SDL_FreeSurface(txtSurface);
    return wh;
}
//
/** \brief rotates one point around another.
* \param pt - your point you want rotated
* \param center - the center point to rotate <pt> around
* \param degrees - the amount of rotation in degrees
* \return a cDoublePt with the rotated point
*/
cDoublePt rotatePoint(cDoublePt pt, cDoublePt center, int degrees)
{

    double s = sin(degToRad(degrees));
    double c = cos(degToRad(degrees));

    pt.x -= center.x;
    pt.y -= center.y;

    double xnew = pt.x * c - pt.y * s;
    double ynew = pt.x * s + pt.y * c;

    pt.x = xnew + center.x;
    pt.y = ynew + center.y;

    return pt;
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
 * \return int - returns number of lines.
 */
int checkFile(char* filePath)
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
    return lines;
}

/** \brief Adds a line of text to the end of a file
 *
 * \param filePath - valid string filepath (relative or absolute)
 * \param stuff - string containing desired text.
 * \return Error code: Code 0: No error. Code -1: Error opening file
 */
int appendLine(char* filePath, char* stuff, bool addNewline)
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
		fprintf(filePtr, (addNewline ? "%s\n" : "%s"), stuff);
		fclose(filePtr);
		return 0;
	}
}

/** \brief inserts a line at a certain position, if the file isn't too big
 *
 * \param
 * \param
 * \param
 * \param
 * \return -1 if failed to open or supply a valid line num, 0 if succeeded
 */
int replaceLine(char* filePath, int lineNum, char* stuff, int maxLength, bool addNewline)
{
    int maxLines = checkFile(filePath) + 1;
    //printf("%d\n", maxLines);
    if (lineNum < 0 || lineNum > maxLines)
        return -1;
    char** allLines = calloc(maxLines, sizeof(char*));
    if (!allLines)
        return -1;
    for(int i = 0; i < maxLines; i++)
    {
        allLines[i] = calloc(maxLength, sizeof(char));
        if (!readLine(filePath, i, maxLength, &(allLines[i])))
            return -1;
        //printf("%s\n", allLines[i]);
    }

    strncpy(allLines[lineNum], stuff, maxLength);
    if (addNewline)
        strncat(allLines[lineNum], "\n", maxLength);
    //printf("%s at %d\n", allLines[lineNum], lineNum);

    createFile(filePath);
    for(int i = 0; i < maxLines; i++)
    {
        if (appendLine(filePath, allLines[i], false) == -1)
            return -1;
        //printf("%s\n", allLines[i]);
    }

    return 0;
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
	if (!filePtr || !*output)
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
