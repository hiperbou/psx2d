
#include <stdbool.h>
#include "hgl_ent.h"
#include "actor.h"

#define MIN_POSX        FIX32(10)
#define MAX_POSX        FIX32(400)
#define MAX_POSY        FIX32(156)

void updatePhysicsActorSimple(Actor *actor, fix32 speed) {
    HGL_Entity * ent = actor->entity;
    fix32 posx = ent->x;
    fix32 posy = ent->y;

	u16 order = actor->enemy.direction;
	// enemies physic
    if (order > 0) posx += speed;
    else posx -= speed;

    if ((posx >= MAX_POSX) || (posx <= MIN_POSX))
            actor->enemy.direction = !order;

    HGL_ENT_setPosition(ent, posx , posy);
}

void faceDirectionAnim(Actor * actor){
    HGL_Entity *entity = actor->entity;
    HGL_SPR_setHFlip(entity->spr, actor->enemy.direction > 0 ? true : false);
}
