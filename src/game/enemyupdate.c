
#include <stdbool.h>
#include "hgl_ent.h"
#include "actor.h"
#include "enemyupdate.h"

void updatePhysicsActorSimple(Actor *actor, fix32 speed) {
    HGL_Entity * ent = actor->entity;
    fix32 posx = ent->x;
    fix32 posy = ent->y;

	u16 order = actor->enemy.direction;
	// enemies physic
    if (order > 0) posx += speed;
    else posx -= speed;

    if ((posx >= actor->enemy.max) || (posx <= actor->enemy.min))
            actor->enemy.direction = !order;

    HGL_ENT_setPosition(ent, posx , posy);
}

void faceDirectionAnim(Actor * actor){
    HGL_Entity *entity = actor->entity;
    HGL_SPR_setHFlip(entity->spr, actor->enemy.direction > 0 ? true : false);
}

enum UpAndDownState updatePhysicsObject(Actor *actor) {
    enum UpAndDownState state = UpAndDownState_updating;

    fix32 y = actor->entity->y;
    fix32 speed = actor->physics.speed;
    y += speed;

    actor->physics.speed += actor->physics.gravity;
    if(speed<0 && actor->physics.speed >= 0) {
        state = UpAndDownState_up;
    }

    if(y >= actor->physics.groundY) {
        state = UpAndDownState_down;
    }

    HGL_ENT_setPosition(actor->entity, actor->entity->x , y);
    return state;
}


enum UpAndDownState updateUpAndDown(Actor *actor, fix32 UP_SPEED, fix32 DOWN_SPEED) {
    enum UpAndDownState state = UpAndDownState_updating;
    fix32 y = actor->entity->y;
    if(actor->enemy.direction == 0) {
        y -= UP_SPEED;
        if(y <= actor->enemy.min) {
            y = actor->enemy.min;
            actor->enemy.direction = 1;
            state = UpAndDownState_up;
        }
    } else if (actor->enemy.direction == 1) {
        y += DOWN_SPEED;
        if(y >= actor->enemy.max) {
            y = actor->enemy.max;
            actor->enemy.direction = 2;
            state = UpAndDownState_down;
        }
    }
    HGL_ENT_setPosition(actor->entity, actor->entity->x , y);
    return state;
}