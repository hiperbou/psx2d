#include "../../core/hgl.h"
#include "../sprites.h"
#include "../../media/fpg.h"


extern struct m_image buffer;
void blit_ex(struct m_image *dest, const struct m_image *src, int px, int py, int sx, int sy, int w, int h);

void draw_sprite(Tsprite * spr) {
    if (spr->graph <= 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    //addRotSprite(spr->x, spr->y, spr->z, spr->angle, spr->size_x, spr->flags, sprite, spr->uploadTpage);
    blit_ex(&buffer, &sprite->m_image, spr->x - (sprite->w >> 1), spr->y  - (sprite->h >> 1), sprite->u, sprite->v, sprite->w, sprite->h);
}

void draw_sprite_fast(Tsprite * spr) {
    if (spr->graph < 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    //addSprite(spr->x, spr->y, spr->z, sprite, spr->uploadTpage);
    blit_ex(&buffer, &sprite->m_image, spr->x - (sprite->w >> 1), spr->y  - (sprite->h >> 1), sprite->u, sprite->v, sprite->w, sprite->h);
}

void draw_tile16_fast(Tsprite * spr) {
    if (spr->graph < 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    //addTile16(spr->x, spr->y, spr->z, sprite);
    blit_ex(&buffer, &sprite->m_image, spr->x, spr->y, sprite->u, sprite->v, sprite->w, sprite->h);
}

void draw_tile8_fast(Tsprite * spr) {
    if (spr->graph < 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    //addTile8(spr->x, spr->y, spr->z, sprite);
    blit_ex(&buffer, &sprite->m_image, spr->x, spr->y, sprite->u, sprite->v, sprite->w, sprite->h);
}