#include "hgl_anim.h"
#include "ObjectPool.h"

#define MAX_NUM_ANIMATION_STATES 64

static ObjectPool * animationStatePool;

void HGL_ANIM_init()
{
    animationStatePool = new_ObjectPool(MAX_NUM_ANIMATION_STATES, sizeof(AnimationState));
}

AnimationState* HGL_ANIM_new()
{
    PoolElement * elem = ObjectPool_get(animationStatePool);
    return elem->data;
}

void HGL_ANIM_delete(AnimationState *e)
{
    ObjectPool_free(animationStatePool, e);
}

void SetAnimationState(AnimationState * animationState, const int* data, int speed) {
    if(animationState->data == data) return;

    *animationState = (AnimationState) {
            .data = (int*) data,
            .frames = (int *) &data[1],
            .numFrames = data[0],
            .animFrame = 0,
            .accumTicks = 0,
            .speed = speed,
            .currentFrame = data[1]
    };
}

static inline void updateAnimationState(AnimationState * animationState) {
    animationState->accumTicks += animationState->speed;
    if (animationState->accumTicks > 100) {
        animationState->animFrame++;
        animationState->animFrame %= animationState->numFrames;
        animationState->currentFrame = animationState->frames[animationState->animFrame];
        animationState->accumTicks -= 100;
    }
}

void HGL_ANIM_updateAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(animationStatePool, AnimationState)
        updateAnimationState(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

