#ifndef SDL3_FPG_H
#define SDL3_FPG_H

#include <SDL3/SDL.h> // For SDL_Texture and other SDL types

// The SPRITE structure for the SDL3 backend.
// This structure holds the SDL_Texture and the necessary information
// to render a specific frame or portion of that texture.
typedef struct SPRITE {
    SDL_Texture *m_image; // Pointer to the SDL_Texture resource.
                          // This texture might be a single image or part of a texture atlas.

    int u;                // The x-coordinate of the top-left corner of the sprite frame
                          // within the m_image texture (for spritesheet/atlas).
    int v;                // The y-coordinate of the top-left corner of the sprite frame
                          // within the m_image texture.

    int w;                // The width of the sprite frame.
    int h;                // The height of the sprite frame.

    // Optional members (can be added if needed by the engine's design):
    // SDL_Color colorKey;    // For color key transparency, if not using alpha blending primarily.
    // float scale_x;         // Default horizontal scale for this sprite.
    // float scale_y;         // Default vertical scale for this sprite.
    // double angle;          // Default rotation angle for this sprite.
    // SDL_Point center;      // Default rotation center for this sprite.
} SPRITE;

#endif // SDL3_FPG_H
