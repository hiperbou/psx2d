
#include <stdbool.h>
#include "hgl_ent.h"
#include "actor.h"

#define MIN_POSX        FIX32(10)
#define MAX_POSX        FIX32(400)
#define MAX_POSY        FIX32(156)

void updatePhysicsActorSimple(HGL_Entity *ent, fix32 speed) {
    fix32 posx = ent->x;
    fix32 posy = ent->y;
    Actor * actor = ent->actor;
	u16 order = actor->enemy.direction;
	// enemies physic
    if (order > 0) posx += speed;
    else posx -= speed;

    if ((posx >= MAX_POSX) || (posx <= MIN_POSX))
            actor->enemy.direction = !order;

    HGL_ENT_setPosition(ent, posx , posy);
}

void faceDirectionAnim(HGL_Entity * entity){
    HGL_SPR_setHFlip(entity->spr, entity->actor->enemy.direction > 0 ? true : false);
}
