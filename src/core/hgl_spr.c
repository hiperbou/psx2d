#include "hgl_spr.h"
/*
#define MAX_SPRITE 40 //MAX_VDP_SPRITE
//Those are defined privately in the SGDK...
//#define VISIBILITY_ALWAYS_FLAG  0x40000000
//#define VISIBILITY_ALWAYS_ON    (VISIBILITY_ALWAYS_FLAG | 0x3FFFFFFF)
//#define VISIBILITY_ALWAYS_OFF   (VISIBILITY_ALWAYS_FLAG | 0x00000000)

typedef struct
{
    u16 w;
    u16 h;
    Palette* palette;
    u16 numAnimation;
    Animation** animations;
    u16 maxNumTile;
    u16 maxNumSprite;
} SpriteDefinition;

typedef struct Sprite
{
    u16 status;
    u16 visibility;
    const SpriteDefinition* definition;
    void (*onFrameChange)(struct Sprite* sprite);
    Animation* animation;
    AnimationFrame* frame;
    s16 animInd;
    s16 frameInd;
    u16 timer;
    s16 x;
    s16 y;
    s16 depth;
    u16 attribut;
    u32 data;
    struct Sprite* prev;
    struct Sprite* next;
} Sprite;

static HGL_Sprite hglSprites[MAX_SPRITE];
static Sprite sprites[MAX_SPRITE];
static PoolElement spriteElement[MAX_SPRITE];
static Pool spritePool;
static u16  totalSprites = 0;

void HGL_SPR_init() {
	//SPR_init(TILE_USERMAXINDEX - HGL_tileVPDIndex, 384 ,256);
	SPR_init(120, TILE_USERMAXINDEX - HGL_tileVPDIndex,0);

	totalSprites = 0;
	HGL_POOL_createPool(&spritePool, MAX_SPRITE, sprites, sizeof(Sprite), spriteElement);
}

void HGL_SPR_update() {
	//SPR_update(sprites, totalSprites);
	SPR_update();
}

//TODO: Attributes, could be just the PAL.
HGL_Sprite* HGL_SPR_new(const SpriteDefinition *sprDefinition, const s16 x, const s16 y, const u16 pal) {
	PoolElement * elem = HGL_POOL_get(&spritePool);
	Sprite * spr = elem->data;
	spr = SPR_addSprite(sprDefinition, x, y, TILE_ATTR(pal, TRUE, FALSE, FALSE));

	totalSprites = spritePool.size;
	HGL_Sprite *hglSprite = &hglSprites[elem->id];
	hglSprite->elem = elem;

	hglSprite->spr = spr;
	return hglSprite;
}

void HGL_SPR_delete(HGL_Sprite * sprite) {
	//sprite->spr->visibility = HIDDEN;
	SPR_releaseSprite(sprite->spr);
	HGL_POOL_free(&spritePool, sprite->elem);
}

void HGL_SPR_setAnim(HGL_Sprite * sprite, u16 anim) {
	SPR_setAnim(sprite->spr, anim);
}

void HGL_SPR_setPosition(HGL_Sprite *sprite, s16 x, s16 y) {
    SPR_setPosition(sprite->spr, x, y);
}

//void HGL_SPR_setAttribut(HGL_Sprite *sprite, u16 attribut) {
//	SPR_setAttribut(sprite->spr, attribut);
//}

void HGL_SPR_setHFlip(HGL_Sprite *sprite, u16 flip){
	SPR_setHFlip(sprite->spr, flip);
}
void HGL_SPR_setVFlip(HGL_Sprite *sprite, u16 flip){
	SPR_setVFlip(sprite->spr, flip);
}
void HGL_SPR_setPalette(HGL_Sprite*sprite, u16 pal){
	SPR_setPalette(sprite->spr, pal);
}
*/
