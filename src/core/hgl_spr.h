#ifndef HGL_SPR_H_
#define HGL_SPR_H_

#include "hgl_pool.h"
#include "hgl_types.h"
#include "sprites.h"

typedef struct{
    Tsprite *spr;
}HGL_Sprite;

void HGL_SPR_init();
void HGL_SPR_update();

void HGL_SPR_renderAll();

HGL_Sprite* HGL_SPR_new(int file, int graph, s16 x, s16 y);

void HGL_SPR_delete(HGL_Sprite * sprite);
void HGL_SPR_setAnim(HGL_Sprite * sprite, u16 anim);
void HGL_SPR_setPosition(HGL_Sprite *sprite, s16 x, s16 y);

void HGL_SPR_setHFlip(HGL_Sprite *sprite, u16 flip);
void HGL_SPR_setVFlip(HGL_Sprite *sprite, u16 flip);

void HGL_SPR_setZ(HGL_Sprite * sprite, int z);

#endif /* HGL_H_ */
