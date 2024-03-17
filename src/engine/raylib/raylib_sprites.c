#include "../../core/hgl.h"
#include "../sprites.h"
#include "../../engine/tilemap.h"
#include "../../media/fpg.h"
#include "../../engine/particle.h"
#include "../../core/hgl_types.h"

#include <raylib.h>

/*
// Texture drawing functions
RLAPI void DrawTexture(Texture2D texture, int posX, int posY, Color tint);                               // Draw a Texture2D
RLAPI void DrawTextureV(Texture2D texture, Vector2 position, Color tint);                                // Draw a Texture2D with position defined as Vector2
RLAPI void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint);  // Draw a Texture2D with extended parameters
RLAPI void DrawTextureRec(Texture2D texture, Rectangle source, Vector2 position, Color tint);            // Draw a part of a texture defined by a rectangle
RLAPI void DrawTexturePro(Texture2D texture, Rectangle source, Rectangle dest, Vector2 origin, float rotation, Color tint); // Draw a part of a texture defined by a rectangle with 'pro' parameters
RLAPI void DrawTextureNPatch(Texture2D texture, NPatchInfo nPatchInfo, Rectangle dest, Vector2 origin, float rotation, Color tint); // Draws a texture (or part of it) that stretches or shrinks nicely
*/

void draw_sprite(Tsprite * spr) {
    if (spr->graph <= 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;

    int mirroredHorizontal = spr->flags & 1;
    int mirroredVertical = spr->flags & 2;

    float scale = spr->size_x / 4096.0;
    int angle = (spr->angle * 360) >> 12; // / 4096;

    Vector2 position = { spr->x, spr->y };
    Rectangle source = { sprite->u, sprite->v,  mirroredHorizontal ? -sprite->w : sprite->w, mirroredVertical ? -sprite->h : sprite->h  };
    Rectangle dest = { position.x, position.y, sprite->w * scale, sprite->h * scale};
    Vector2 origin = { sprite->w >> 1, sprite->h >> 1};

    DrawTexturePro(sprite->m_image, source, dest, origin, angle, WHITE);
}

void draw_sprite_fast(Tsprite * spr) {
    if (spr->graph < 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    Vector2 position = (Vector2) { spr->x - (sprite->w >> 1), spr->y - (sprite->h >> 1) };
    Rectangle source = (Rectangle) { sprite->u, sprite->v, sprite->w, sprite->h  };
    DrawTextureRec(sprite->m_image, source, position, WHITE);
}

void draw_tile16_fast(Tsprite * spr) {
    if (spr->graph < 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    Vector2 position = (Vector2) { spr->x, spr->y };
    Rectangle source = (Rectangle) { sprite->u, sprite->v, sprite->w, sprite->h  };
    DrawTextureRec(sprite->m_image, source, position, WHITE);
}

void draw_tile8_fast(Tsprite * spr) {
    if (spr->graph < 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    Vector2 position = (Vector2) { spr->x, spr->y };
    Rectangle source = (Rectangle) { sprite->u, sprite->v, sprite->w, sprite->h  };
    DrawTextureRec(sprite->m_image, source, position, WHITE);
}