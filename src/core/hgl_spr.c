#include "hgl_spr.h"
#include "../pool/ObjectPool.h"

#define MAX_HGL_SPRITES 40

static ObjectPool * spritePool;

void HGL_SPR_init() {
    spritePool = new_ObjectPool(MAX_HGL_SPRITES, sizeof(HGL_Sprite));
}

void HGL_SPR_update() {

}

static inline void HGL_SPR_render(HGL_Sprite *sprite)
{
    int spriteX = sprite->x;
    int spriteY = sprite->y;
    if (spriteX < 0 || spriteX > 320 || spriteY < 0 || spriteY > 240) { //TODO: proper out of bounds
        if (sprite->spr) {
            delete_sprite(sprite->spr);
            sprite->spr = NULL;
        }
        return;
    } else if (sprite->spr==NULL) {
        sprite->spr = new_sprite(sprite->x, sprite->y, sprite->z, sprite->file, sprite->graph);
    }

    if(sprite->spr == NULL) return;

    sprite->spr->x = sprite->x;
    sprite->spr->y = sprite->y;
    sprite->spr->z = sprite->z;
    sprite->spr->file = sprite->file;
    sprite->spr->graph = sprite->graph;
    sprite->spr->flags = sprite->flags;
}

void HGL_SPR_renderAll()
{
    OBJECTPOOL_ITERATOR_ALLOCATED_START(spritePool, HGL_Sprite)
        HGL_SPR_render(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

HGL_Sprite* HGL_SPR_new(const int file, const int graph, const s16 x, const s16 y) {
    PoolElement * elem = ObjectPool_get(spritePool);
    HGL_Sprite * hglSprite = elem->data;

    *hglSprite = (HGL_Sprite) {
        .file = file,
        .graph = graph,
        .x = x,
        .y = y,
        .z = 0,
        .flags = 0
    };

    return hglSprite;
}

void HGL_SPR_delete(HGL_Sprite * sprite) {
    //Clear object
    if (sprite->spr) {
        delete_sprite(sprite->spr);
        sprite->spr = NULL;
    }
    ObjectPool_free(spritePool, sprite);
}

void HGL_SPR_setAnim(HGL_Sprite * sprite, u16 anim) {
	//SPR_setAnim(sprite->spr, anim);
}

void HGL_SPR_setPosition(HGL_Sprite *sprite, s16 x, s16 y) {
    sprite->x = x;
    sprite->y = y;
}

void HGL_SPR_setHFlip(HGL_Sprite *sprite, u16 flip){
    if (flip)
        sprite->flags |= 1;
    else
        sprite->flags &= ~1;
}

void HGL_SPR_setVFlip(HGL_Sprite *sprite, u16 flip){
    if (flip)
        sprite->flags |= 2;
    else
        sprite->flags &= ~2;

}

void HGL_SPR_setZ(HGL_Sprite * sprite, int z) {
    sprite->z = z;
}