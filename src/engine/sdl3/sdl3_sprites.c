#include <SDL3/SDL.h>

#include "../../core/hgl.h"       // For general HGL stuff
#include "../sprites.h"         // For Tsprite definition
#include "../../engine/tilemap.h" // For context, may not be directly used
#include "../../media/fpg.h"      // For FPG_ST and SPRITE definitions
#include "../../core/hgl_types.h" // For HGL specific types

// Assume 'renderer' is an SDL_Renderer* initialized in sdl3_hgl.c
// and made available globally or via an accessor.
// For this implementation, we'll declare it as extern.
extern SDL_Renderer *renderer; 

// Assume 'fpg' array is global or accessible as defined in the project.
// This typically holds pointers to loaded FPG_ST structures.
extern FPG_ST *fpg[MAX_FPGS]; 

void draw_sprite(Tsprite *spr) {
    if (!spr) {
        SDL_Log("draw_sprite: spr is NULL");
        return;
    }
    if (spr->graph <= 0 || spr->id < 0) {
        // SDL_Log("draw_sprite: Invalid graph or id (graph: %d, id: %d)", spr->graph, spr->id);
        return; // Common to not log this if it's an expected "not visible" state
    }
    if (spr->file < 0 || spr->file >= MAX_FPGS || !fpg[spr->file] || !fpg[spr->file]->map[spr->graph]) {
        SDL_Log("draw_sprite: Invalid fpg file index or map data (file: %d, graph: %d)", spr->file, spr->graph);
        return;
    }

    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    if (!sprite || !sprite->m_image) {
        SDL_Log("draw_sprite: Sprite image or texture is NULL (file: %d, graph: %d)", spr->file, spr->graph);
        return;
    }
    if (!renderer) {
        SDL_Log("draw_sprite: renderer is NULL!");
        return;
    }

    int mirroredHorizontal = spr->flags & 1;
    int mirroredVertical = spr->flags & 2;

    float scale = spr->size_x / 4096.0f;
    if (spr->size_x == 0) scale = 1.0f; // Default to 1.0 scale if size_x is 0

    double angle = (spr->angle * 360.0) / 4096.0;

    SDL_Rect sourceRect;
    sourceRect.x = sprite->u;
    sourceRect.y = sprite->v;
    sourceRect.w = sprite->w;
    sourceRect.h = sprite->h;

    SDL_FRect destRect;
    // Assuming spr->x and spr->y are the center of the sprite
    destRect.x = spr->x - (sprite->w * scale / 2.0f);
    destRect.y = spr->y - (sprite->h * scale / 2.0f);
    destRect.w = sprite->w * scale;
    destRect.h = sprite->h * scale;

    SDL_FPoint center;
    // Center for rotation is relative to the destination rectangle
    center.x = destRect.w / 2.0f; 
    center.y = destRect.h / 2.0f;

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (mirroredHorizontal) {
        flip |= SDL_FLIP_HORIZONTAL;
    }
    if (mirroredVertical) {
        flip |= SDL_FLIP_VERTICAL;
    }

    // Tsprite alpha: 0=opaque, 255=fully transparent
    // SDL_SetTextureAlphaMod: 0=fully transparent, 255=opaque
    Uint8 sdl_alpha = 255; // Default to opaque
    if (spr->alpha > 0 && spr->alpha < 255) { // If alpha is 0, it's fully opaque. If 255, fully transparent.
        sdl_alpha = 255 - spr->alpha;
    } else if (spr->alpha == 255) { // Fully transparent
        sdl_alpha = 0;
    }
    // It's good practice to set alpha before each draw if textures are shared and alpha can vary
    SDL_SetTextureAlphaMod(sprite->m_image, sdl_alpha);

    if (SDL_RenderTextureRotated(renderer, sprite->m_image, &sourceRect, &destRect, angle, &center, flip) != 0) {
         SDL_Log("draw_sprite: SDL_RenderTextureRotated failed: %s", SDL_GetError());
    }
    
    // Optional: Reset alpha mod if other parts of the code expect textures to be fully opaque by default.
    // However, it's generally better to set it before each draw call.
    // SDL_SetTextureAlphaMod(sprite->m_image, 255); 
}

void draw_sprite_fast(Tsprite *spr) {
    if (!spr) {
        SDL_Log("draw_sprite_fast: spr is NULL");
        return;
    }
    if (spr->graph <= 0 || spr->id < 0) {
        return;
    }
    if (spr->file < 0 || spr->file >= MAX_FPGS || !fpg[spr->file] || !fpg[spr->file]->map[spr->graph]) {
         SDL_Log("draw_sprite_fast: Invalid fpg file index or map data (file: %d, graph: %d)", spr->file, spr->graph);
        return;
    }

    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    if (!sprite || !sprite->m_image) {
        SDL_Log("draw_sprite_fast: Sprite image or texture is NULL (file: %d, graph: %d)", spr->file, spr->graph);
        return;
    }
    if (!renderer) {
        SDL_Log("draw_sprite_fast: renderer is NULL!");
        return;
    }

    SDL_Rect sourceRect;
    sourceRect.x = sprite->u;
    sourceRect.y = sprite->v;
    sourceRect.w = sprite->w;
    sourceRect.h = sprite->h;

    SDL_FRect destRect;
    // Assuming spr->x, spr->y are center points for consistency with draw_sprite
    destRect.x = spr->x - (sprite->w >> 1); // sprite->w / 2
    destRect.y = spr->y - (sprite->h >> 1); // sprite->h / 2
    destRect.w = sprite->w;
    destRect.h = sprite->h;
    
    Uint8 sdl_alpha = 255;
    if (spr->alpha > 0 && spr->alpha < 255) {
        sdl_alpha = 255 - spr->alpha;
    } else if (spr->alpha == 255) {
        sdl_alpha = 0;
    }
    SDL_SetTextureAlphaMod(sprite->m_image, sdl_alpha);

    if (SDL_RenderTexture(renderer, sprite->m_image, &sourceRect, &destRect) != 0) {
        SDL_Log("draw_sprite_fast: SDL_RenderTexture failed: %s", SDL_GetError());
    }
}

void draw_tile16_fast(Tsprite *spr) {
    if (!spr) {
        SDL_Log("draw_tile16_fast: spr is NULL");
        return;
    }
    if (spr->graph <= 0 || spr->id < 0) {
        return;
    }
    if (spr->file < 0 || spr->file >= MAX_FPGS || !fpg[spr->file] || !fpg[spr->file]->map[spr->graph]) {
        SDL_Log("draw_tile16_fast: Invalid fpg file index or map data (file: %d, graph: %d)", spr->file, spr->graph);
        return;
    }

    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    if (!sprite || !sprite->m_image) {
        SDL_Log("draw_tile16_fast: Sprite image or texture is NULL (file: %d, graph: %d)", spr->file, spr->graph);
        return;
    }
    if (!renderer) {
        SDL_Log("draw_tile16_fast: renderer is NULL!");
        return;
    }

    SDL_Rect sourceRect;
    sourceRect.x = sprite->u; 
    sourceRect.y = sprite->v;
    sourceRect.w = sprite->w; // Assuming sprite->w is 16 for a 16x16 tile
    sourceRect.h = sprite->h; // Assuming sprite->h is 16 for a 16x16 tile

    SDL_FRect destRect;
    destRect.x = spr->x; // Tile X is top-left
    destRect.y = spr->y; // Tile Y is top-left
    destRect.w = sprite->w; // Or fixed 16: 16.0f;
    destRect.h = sprite->h; // Or fixed 16: 16.0f;
    
    Uint8 sdl_alpha = 255;
    if (spr->alpha > 0 && spr->alpha < 255) {
        sdl_alpha = 255 - spr->alpha;
    } else if (spr->alpha == 255) {
        sdl_alpha = 0;
    }
    SDL_SetTextureAlphaMod(sprite->m_image, sdl_alpha);

    if (SDL_RenderTexture(renderer, sprite->m_image, &sourceRect, &destRect) != 0) {
         SDL_Log("draw_tile16_fast: SDL_RenderTexture failed: %s", SDL_GetError());
    }
}

void draw_tile8_fast(Tsprite *spr) {
    if (!spr) {
        SDL_Log("draw_tile8_fast: spr is NULL");
        return;
    }
    if (spr->graph <= 0 || spr->id < 0) {
        return;
    }
     if (spr->file < 0 || spr->file >= MAX_FPGS || !fpg[spr->file] || !fpg[spr->file]->map[spr->graph]) {
        SDL_Log("draw_tile8_fast: Invalid fpg file index or map data (file: %d, graph: %d)", spr->file, spr->graph);
        return;
    }

    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    if (!sprite || !sprite->m_image) {
        SDL_Log("draw_tile8_fast: Sprite image or texture is NULL (file: %d, graph: %d)", spr->file, spr->graph);
        return;
    }
    if (!renderer) {
        SDL_Log("draw_tile8_fast: renderer is NULL!");
        return;
    }

    SDL_Rect sourceRect;
    sourceRect.x = sprite->u;
    sourceRect.y = sprite->v;
    sourceRect.w = sprite->w; // Assuming sprite->w is 8
    sourceRect.h = sprite->h; // Assuming sprite->h is 8

    SDL_FRect destRect;
    destRect.x = spr->x; // Tile X is top-left
    destRect.y = spr->y; // Tile Y is top-left
    destRect.w = sprite->w; // Or fixed 8: 8.0f;
    destRect.h = sprite->h; // Or fixed 8: 8.0f;

    Uint8 sdl_alpha = 255;
    if (spr->alpha > 0 && spr->alpha < 255) {
        sdl_alpha = 255 - spr->alpha;
    } else if (spr->alpha == 255) {
        sdl_alpha = 0;
    }
    SDL_SetTextureAlphaMod(sprite->m_image, sdl_alpha);

    if (SDL_RenderTexture(renderer, sprite->m_image, &sourceRect, &destRect) != 0) {
        SDL_Log("draw_tile8_fast: SDL_RenderTexture failed: %s", SDL_GetError());
    }
}

// initSprites() is declared in sprites.h.
// If an SDL3-specific implementation is needed, it could be defined here.
// For now, this file focuses on drawing.
// The generic initSprites in engine/sprites.c is likely sufficient.
/*
void initSprites() {
    // SDL3 specific sprite system initialization if any.
    // For example, if we used a sprite batching system specific to SDL3.
    // Currently, fpg array and renderer are initialized elsewhere.
    SDL_Log("SDL3 initSprites (sdl3_sprites.c) called - if needed.\n");
}
*/
