#ifndef ENEMYUPDATE_H_
#define ENEMYUPDATE_H_

#include "hgl_ent.h"

void updatePhysicsActorSimple(HGL_Entity *ent, fix32 speed);
void faceDirectionAnim(HGL_Entity * entity);

static inline void setAnimation(HGL_Entity * entity, const int * data, int speed) {
    SetAnimationState(entity->animationState, data, speed);
}

#endif /* ENEMYUPDATE_H_ */
