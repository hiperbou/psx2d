#ifndef ENEMYUPDATE_H_
#define ENEMYUPDATE_H_

#include "hgl_ent.h"
#include "actor.h"

void updatePhysicsActorSimple(Actor *ent, fix32 speed);
void faceDirectionAnim(Actor * entity);

static inline void setAnimation(Actor * actor, const int * data, int delay) {
    SetAnimationState(actor->animationState, data, delay);
}

static inline void setAnimationDelay(Actor*actor, int delay) {
    SetAnimationDelay(actor->animationState, delay);
}

#endif /* ENEMYUPDATE_H_ */
