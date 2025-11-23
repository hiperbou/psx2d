#ifndef SDL3_BACKEND_H
#define SDL3_BACKEND_H

#include <SDL3/SDL.h>

typedef struct SPRITE {
    SDL_Texture* texture;
    int u, v;
    int w, h;
    int tex_w, tex_h;
} SPRITE;

#endif // SDL3_BACKEND_H
