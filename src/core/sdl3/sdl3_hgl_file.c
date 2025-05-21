#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h> // For IMG_LoadTexture and IMG_LoadTexture_RW

#include "../hgl_file.h"      // Assumed to contain SPRITE definition compatible with SDL_Texture*
#include "../../utils/utils.h" // For utility functions if any are needed (like string manipulation)
#include "sdl3_hgl.h"         // For HGL_GetRenderer()

#include <string.h> // For strcpy, strcat

// Fills a SPRITE structure with information from an SDL_Texture
void GetSprite(SDL_Texture *texture, SPRITE *sprite) {
    if (!sprite || !texture) {
        SDL_Log("GetSprite: Null sprite or texture pointer provided.");
        return;
    }

    sprite->texture = texture;

    // Get texture dimensions
    if (SDL_QueryTexture(texture, NULL, NULL, &sprite->w, &sprite->h) != 0) {
        SDL_Log("GetSprite: Failed to query texture attributes: %s", SDL_GetError());
        // Potentially nullify texture or set w/h to 0 to indicate error
        sprite->texture = NULL; 
        sprite->w = 0;
        sprite->h = 0;
    }

    // Assuming u, v are for spritesheet offsets, default to 0 for full texture
    sprite->u = 0;
    sprite->v = 0;
}

void GetSpriteFromDisc(const char *filename, SPRITE *sprite) {
    if (!filename || !sprite) {
        SDL_Log("GetSpriteFromDisc: Null filename or sprite pointer provided.");
        return;
    }

    char fullFilename[256]; // Buffer for filename manipulation
    SDL_Renderer* renderer = HGL_GetRenderer();

    if (!renderer) {
        SDL_Log("GetSpriteFromDisc: Failed to get SDL_Renderer.");
        sprite->texture = NULL;
        sprite->w = 0;
        sprite->h = 0;
        return;
    }

    // Construct the full path, assuming files are relative to a known directory
    // or that filename includes path. For now, just append .png as in original.
    // Consider using SDL_GetBasePath() + filename for more robust pathing.
    strcpy(fullFilename, filename);
    strcat(fullFilename, ".png"); // Assuming all sprites are PNGs as per original

    SDL_Log("GetSpriteFromDisc: Loading texture from %s", fullFilename);
    SDL_Texture *texture = IMG_LoadTexture(renderer, fullFilename);

    if (texture == NULL) {
        SDL_Log("GetSpriteFromDisc: Failed to load texture '%s': %s", fullFilename, IMG_GetError());
        sprite->texture = NULL;
        sprite->w = 0;
        sprite->h = 0;
        return;
    }

    GetSprite(texture, sprite);
}

void GetSpriteFromMemory(const uint8_t *data, size_t data_size, SPRITE *sprite) {
    if (!data || data_size == 0 || !sprite) {
        SDL_Log("GetSpriteFromMemory: Null data, zero size, or null sprite pointer provided.");
        return;
    }

    SDL_Renderer* renderer = HGL_GetRenderer();
    if (!renderer) {
        SDL_Log("GetSpriteFromMemory: Failed to get SDL_Renderer.");
        sprite->texture = NULL;
        sprite->w = 0;
        sprite->h = 0;
        return;
    }

    // Create an SDL_RWops from the memory buffer.
    // SDL_RWFromConstMem is appropriate as we are just reading.
    SDL_RWops *rw = SDL_RWFromConstMem(data, (int)data_size);
    if (rw == NULL) {
        SDL_Log("GetSpriteFromMemory: Failed to create SDL_RWops from memory: %s", SDL_GetError());
        sprite->texture = NULL;
        sprite->w = 0;
        sprite->h = 0;
        return;
    }

    // IMG_LoadTexture_RW can load various formats if SDL_image supports them.
    // The second argument '1' to IMG_LoadTexture_RW means to automatically close/free the RWops.
    SDL_Texture *texture = IMG_LoadTexture_RW(renderer, rw, 1); 

    if (texture == NULL) {
        SDL_Log("GetSpriteFromMemory: Failed to load texture from memory: %s", IMG_GetError());
        sprite->texture = NULL;
        sprite->w = 0;
        sprite->h = 0;
        // rw is freed by IMG_LoadTexture_RW if '1' is passed, otherwise SDL_DestroyRW(rw);
        return;
    }

    GetSprite(texture, sprite);
}

// It's good practice to have a way to free sprites if they own textures.
// This wasn't in the original raylib_hgl_file.c but is important for SDL.
void FreeSprite(SPRITE *sprite) {
    if (sprite && sprite->texture) {
        SDL_DestroyTexture(sprite->texture);
        sprite->texture = NULL;
        sprite->w = 0;
        sprite->h = 0;
    }
}
```
