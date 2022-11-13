#ifndef ENEMYUPDATE_H_
#define ENEMYUPDATE_H_

#include "hgl_ent.h"
#include "actor.h"

void updatePhysicsActorSimple(Actor *ent, fix32 speed);
void faceDirectionAnim(Actor * entity);


enum UpAndDownState {
    UpAndDownState_updating, UpAndDownState_start, UpAndDownState_up, UpAndDownState_down
};

enum UpAndDownState updateUpAndDown(Actor *actor, fix32 UP_SPEED, fix32 DOWN_SPEED);

static inline void setAnimation(Actor * actor, const int * data, int delay) {
    SetAnimationState(actor->animationState, data, delay);
}

static inline void setZ(Actor * actor, int z) {
    HGL_SPR_setZ(actor->entity->spr, z);
}

static inline void setAnimationDelay(Actor*actor, int delay) {
    SetAnimationDelay(actor->animationState, delay);
}

#endif /* ENEMYUPDATE_H_ */
