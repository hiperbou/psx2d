#ifndef SDL3_FPG_H
#define SDL3_FPG_H

// It's good practice to include SDL headers carefully.
// If SDL.h is too broad, specific headers like SDL_render.h for SDL_Texture might be used.
// However, SDL.h is common.
#include <SDL3/SDL.h> 

typedef struct SPRITE {
    SDL_Texture* texture; // Pointer to the SDL texture
    int u;                // X offset in the texture (for sprite sheets)
    int v;                // Y offset in the texture (for sprite sheets)
    int w;                // Width of the sprite
    int h;                // Height of the sprite
} SPRITE;

#endif //SDL3_FPG_H
