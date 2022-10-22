#ifndef HGL_ENT_H
#define HGL_ENT_H

#include "hgl_spr.h"
#include "hgl_types.h"
#include "hgl_anim.h"

typedef enum {
    ENT_STATE_DEAD,
    ENT_STATE_ACTIVE,
    ENT_STATE_INVISIBLE
} EntityState;

typedef struct Actor Actor;

typedef struct HGL_Entity HGL_Entity;
typedef void EntityUpdateCallback(HGL_Entity* entityDef);

typedef struct HGL_Entity{
    fix32 x;
    fix32 y;
    EntityState state;
    u16 id;
    u16 user0;
    HGL_Sprite *spr;
    AnimationState *animationState;
    Actor *actor;
    EntityUpdateCallback *updateCallback;
} HGL_Entity;



void        HGL_ENT_init();


HGL_Entity* HGL_ENT_new2(const fix32 x, const fix32 y, HGL_Sprite*spr, AnimationState *animationState, EntityUpdateCallback* updateCB, Actor *actor);
void        HGL_ENT_delete(HGL_Entity *e);
void        HGL_ENT_update(HGL_Entity *e);
void        HGL_ENT_updateAll(u16 offsetX,u16 offsetY);
void        HGL_ENT_renderAll(u16 offsetX,u16 offsetY);
void 		HGL_ENT_setPosition(HGL_Entity *e, fix32 x, fix32 y);

inline HGL_Entity* HGL_ENT_new(const fix32 x, const fix32 y, HGL_Sprite*spr, EntityUpdateCallback* updateCB){
	return HGL_ENT_new2(x, y, spr, NULL, updateCB, NULL);
}

#endif
