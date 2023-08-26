#include "../core/hgl.h"
#include "../core/hgl_spr.h"
#include "../core/hgl_ent.h"

#include "enemyupdate.h"
#include "../core/hgl_actor.h"
#include "../core/hgl_anim.h"

ANIM(anim_patrol, 5, 6, 7, 8)

static void update(Actor* actor) {
	updatePhysicsActorSimple(actor, FIX32(0.7));
}

static void constructor(Actor* actor) {
    setAnimation(actor, anim_patrol, 10);
    setZ(actor, 1);
    actor->enemy.direction = 0;
    actor->enemy.min = actor->entity->x - FIX32(64);
    actor->enemy.max = actor->entity->x + FIX32(64);
}

Actor* newMotobug(int file, const fix32 x, const fix32 y){
	return newActor(file, 5, x, y, constructor, update);
}

