#include <SDL3/SDL.h>

#include "../../core/sdl3/sdl3_hgl.h" // For HGL_GetRenderer()
#include "../sprites.h"             // For Tsprite definition
#include "../../core/hgl_file.h"    // Assumed SPRITE definition with SDL_Texture*
#include "../../media/fpg.h"        // For fpg array and FPG_FILE, MAP_POINT structures

// Forward declaration if not in a common header accessible here, or ensure hgl_file.h has it
// typedef struct {
//     SDL_Texture* texture;
//     int w, h, u, v;
// } SPRITE;


void draw_sprite(Tsprite * spr) {
    if (!spr || spr->graph <= 0 || spr->id < 0 || !fpg[spr->file] || !fpg[spr->file]->map[spr->graph]) return;

    SPRITE* sprite_data = fpg[spr->file]->map[spr->graph]->image;
    if (!sprite_data || !sprite_data->texture) return;

    SDL_Renderer* renderer = HGL_GetRenderer();
    if (!renderer) return;

    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (spr->flags & 1) flip = (SDL_RendererFlip)(flip | SDL_FLIP_HORIZONTAL);
    if (spr->flags & 2) flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);

    float scale = spr->size_x / 4096.0f;
    // Angle in Raylib is (val * 360) >> 12 which is (val * 360) / 4096
    double angle = (spr->angle * 360.0) / 4096.0;

    SDL_Rect srcRect;
    srcRect.x = sprite_data->u;
    srcRect.y = sprite_data->v;
    srcRect.w = sprite_data->w;
    srcRect.h = sprite_data->h;

    SDL_FRect dstRect;
    dstRect.w = (float)sprite_data->w * scale;
    dstRect.h = (float)sprite_data->h * scale;

    // SDL_FPoint defines the center of rotation relative to the dstRect.
    // We want spr->x, spr->y to be the center of the sprite.
    SDL_FPoint center;
    center.x = dstRect.w / 2.0f;
    center.y = dstRect.h / 2.0f;

    // Adjust dstRect position so that (spr->x, spr->y) is the pivot point (center of the sprite)
    dstRect.x = (float)spr->x - center.x;
    dstRect.y = (float)spr->y - center.y;
    
    // Raylib's DrawTexturePro uses WHITE for no tint. SDL_SetTextureColorMod can be used for tinting.
    // For default (no tint), we don't need to call SDL_SetTextureColorMod.
    // Ensure texture color mod is reset if other parts of code change it.
    // For now, assume it's in the default state (no tint).
    // SDL_SetTextureColorMod(sprite_data->texture, 255, 255, 255); // If explicit reset is needed

    SDL_RenderTextureRotated(renderer, sprite_data->texture, &srcRect, &dstRect, angle, &center, flip);
    // Or SDL_RenderCopyExF(renderer, sprite_data->texture, &srcRect, &dstRect, angle, &center, flip);
    // SDL_RenderTextureRotated is often a macro for SDL_RenderCopyExF with float rects
}

void draw_sprite_fast(Tsprite * spr) {
    if (!spr || spr->graph < 0 || spr->id < 0 || !fpg[spr->file] || !fpg[spr->file]->map[spr->graph]) return;
    
    SPRITE* sprite_data = fpg[spr->file]->map[spr->graph]->image;
    if (!sprite_data || !sprite_data->texture) return;

    SDL_Renderer* renderer = HGL_GetRenderer();
    if (!renderer) return;

    SDL_Rect srcRect;
    srcRect.x = sprite_data->u;
    srcRect.y = sprite_data->v;
    srcRect.w = sprite_data->w;
    srcRect.h = sprite_data->h;

    SDL_Rect dstRect;
    // Raylib's DrawTextureRec draws with position as top-left.
    // The original raylib_sprites.c has: position = { spr->x - (sprite->w >> 1), spr->y - (sprite->h >> 1) }
    // This means spr->x, spr->y is the center.
    dstRect.x = spr->x - (sprite_data->w >> 1);
    dstRect.y = spr->y - (sprite_data->h >> 1);
    dstRect.w = sprite_data->w;
    dstRect.h = sprite_data->h;

    SDL_RenderTexture(renderer, sprite_data->texture, &srcRect, &dstRect);
    // Or SDL_RenderCopy(renderer, sprite_data->texture, &srcRect, &dstRect);
}

void draw_tile16_fast(Tsprite * spr) {
    if (!spr || spr->graph < 0 || spr->id < 0 || !fpg[spr->file] || !fpg[spr->file]->map[spr->graph]) return;

    SPRITE* sprite_data = fpg[spr->file]->map[spr->graph]->image;
    if (!sprite_data || !sprite_data->texture) return;

    SDL_Renderer* renderer = HGL_GetRenderer();
    if (!renderer) return;

    SDL_Rect srcRect;
    srcRect.x = sprite_data->u;
    srcRect.y = sprite_data->v;
    srcRect.w = sprite_data->w; // Should be 16 for 16x16 tiles
    srcRect.h = sprite_data->h; // Should be 16 for 16x16 tiles

    SDL_Rect dstRect;
    // Raylib's DrawTextureRec uses position as top-left. Original: { spr->x, spr->y }
    dstRect.x = spr->x;
    dstRect.y = spr->y;
    dstRect.w = sprite_data->w; // Or fixed 16
    dstRect.h = sprite_data->h; // Or fixed 16
    
    // If these are strictly tiles, sprite_data->w and sprite_data->h should match tile dimensions.
    // If sprite_data can be larger and only a part is a tile, srcRect needs adjustment.
    // Assuming sprite_data->w/h IS the tile dimension here.

    SDL_RenderTexture(renderer, sprite_data->texture, &srcRect, &dstRect);
}

void draw_tile8_fast(Tsprite * spr) {
    if (!spr || spr->graph < 0 || spr->id < 0 || !fpg[spr->file] || !fpg[spr->file]->map[spr->graph]) return;

    SPRITE* sprite_data = fpg[spr->file]->map[spr->graph]->image;
    if (!sprite_data || !sprite_data->texture) return;

    SDL_Renderer* renderer = HGL_GetRenderer();
    if (!renderer) return;

    SDL_Rect srcRect;
    srcRect.x = sprite_data->u;
    srcRect.y = sprite_data->v;
    srcRect.w = sprite_data->w; // Should be 8
    srcRect.h = sprite_data->h; // Should be 8

    SDL_Rect dstRect;
    dstRect.x = spr->x;
    dstRect.y = spr->y;
    dstRect.w = sprite_data->w; // Or fixed 8
    dstRect.h = sprite_data->h; // Or fixed 8

    SDL_RenderTexture(renderer, sprite_data->texture, &srcRect, &dstRect);
}

// A function to initialize sprite system if needed (e.g. load default sprite, init sprite list)
void initSprites() {
    // Currently, fpg data is loaded elsewhere.
    // If there's any SDL-specific setup for sprites, it would go here.
    // For now, this can be empty or just a log message.
    SDL_Log("SDL Sprite Engine Initialized (sprites rely on FPG data).");
}

// A function to clean up sprite resources if any were allocated by this module
void closeSprites() {
    // If this module allocated any specific SDL resources for sprites (e.g. default textures), free them.
    // Note: Individual sprite textures loaded via fpgs are typically managed by fpg module or GetSprite/FreeSprite.
    SDL_Log("SDL Sprite Engine Closed.");
}

```
