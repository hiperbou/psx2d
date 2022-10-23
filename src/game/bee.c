#include "hgl.h"
#include "hgl_spr.h"
#include "hgl_ent.h"

#include "enemyupdate.h"
#include "actor.h"
#include "hgl_anim.h"

ANIM(anim_patrol, 1, 2)

static void updatePhysics(Actor *ent) {
	updatePhysicsActorSimple(ent, FIX32(1.7));
}

static void update(Actor* ent) {
	updatePhysics(ent);
	faceDirectionAnim(ent);//TODO: call faceDirection only on direction change :D
}

static void constructor(Actor* actor) {
    setAnimation(actor, anim_patrol, 2);
}

Actor* newBee(int file, const fix32 x, const fix32 y) {
	return newActor(file,1, x, y,constructor,update);
}

