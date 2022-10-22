#include "hgl.h"
#include "hgl_spr.h"
#include "hgl_ent.h"

#include "enemyupdate.h"
#include "actor.h"
#include "hgl_anim.h"

static const int anim_patrol[] = {2, 1, 2}; //The first number indicates the number of frames

static void updatePhysics(HGL_Entity *ent) {
	updatePhysicsActorSimple(ent, FIX32(1.7));
}

static void update(HGL_Entity* ent) {
	updatePhysics(ent);
	faceDirectionAnim(ent);//TODO: call faceDirection only on direction change :D
}

static void constructor(Actor* actor) {
    setAnimation(actor->entity, anim_patrol, 50);
}

Actor* newBee(int file, const fix32 x, const fix32 y) {
	return newActor(file,1, x, y,constructor,update);
}

