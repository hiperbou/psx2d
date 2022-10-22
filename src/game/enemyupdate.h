#ifndef ENEMYUPDATE_H_
#define ENEMYUPDATE_H_

#include "hgl_ent.h"
#include "actor.h"

void updatePhysicsActorSimple(Actor *ent, fix32 speed);
void faceDirectionAnim(Actor * entity);

static inline void setAnimation(Actor * actor, const int * data, int speed) {
    SetAnimationState(actor->animationState, data, speed);
}

#endif /* ENEMYUPDATE_H_ */
