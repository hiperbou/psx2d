#include "hgl_spr.h"
#include "ObjectPool.h"

#define MAX_SPRITES 40

static ObjectPool * spritePool;

void HGL_SPR_init() {
    spritePool = new_ObjectPool(MAX_SPRITES, sizeof(HGL_Sprite));
}

void HGL_SPR_update() {

}

static inline void HGL_SPR_render(HGL_Sprite *e)
{
    draw_sprite(e->spr);
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

    /*hglSprite->spr = (Tsprite) {
        .file = file,
        .graph = graph,
        .x = 0,
        .y = 0
    };*/
    hglSprite->spr = new_sprite(x,y,0,file,graph);

    return hglSprite;
}

void HGL_SPR_delete(HGL_Sprite * sprite) {
    ObjectPool_free(spritePool, sprite);
}

void HGL_SPR_setAnim(HGL_Sprite * sprite, u16 anim) {
	//SPR_setAnim(sprite->spr, anim);
}

void HGL_SPR_setPosition(HGL_Sprite *sprite, s16 x, s16 y) {
    //SPR_setPosition(sprite->spr, x, y);
    sprite->spr->x = x;
    sprite->spr->y = y;
}

void HGL_SPR_setHFlip(HGL_Sprite *sprite, u16 flip){
    if (flip)
        sprite->spr->flags |= 1;
    else
        sprite->spr->flags &= ~1;
}

void HGL_SPR_setVFlip(HGL_Sprite *sprite, u16 flip){
    if (flip)
        sprite->spr->flags |= 2;
    else
        sprite->spr->flags &= ~2;

}
