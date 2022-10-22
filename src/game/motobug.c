#include "hgl.h"
#include "hgl_spr.h"
#include "hgl_ent.h"

#include "enemyupdate.h"
#include "actor.h"
#include "hgl_anim.h"

static const int anim_patrol[] = {4, 5, 6, 7, 8}; //The first number indicates the number of frames

static void update(HGL_Entity* ent) {
	updatePhysicsActorSimple(ent, FIX32(0.7));
}

static void constructor(Actor* actor) {
	HGL_SPR_setAnim(actor->entity->spr, 1);
    setAnimation(actor->entity, anim_patrol, 10);
}

Actor* newMotobug(int file, const fix32 x, const fix32 y){
	return newActor(file, 5, x, y, constructor, update);
}

