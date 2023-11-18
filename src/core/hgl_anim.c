#include "hgl_anim.h"
#include "../pool/ObjectPool.h"

#define MAX_NUM_ANIMATION_STATES 64

static ObjectPool * animationStatePool;

void HGL_ANIM_init()
{
    animationStatePool = new_ObjectPool(MAX_NUM_ANIMATION_STATES, sizeof(AnimationState));
}

ANIM(Empty, 0)
inline static void HGL_ANIM_reset(AnimationState *animationState) {
    SetAnimationState(animationState, Empty, 32);
}

AnimationState* HGL_ANIM_new()
{
    PoolElement * elem = ObjectPool_get(animationStatePool);
    HGL_ANIM_reset(elem->data);
    return elem->data;
}

void HGL_ANIM_delete(AnimationState *animationState)
{
    ObjectPool_free(animationStatePool, animationState);
}

void SetAnimationState(AnimationState * animationState, const int* data, int delay) {
    if(animationState->data == data) return;

    *animationState = (AnimationState) {
            .data = (int*) data,
            .frames = ANIM_GET_FRAMES_POINTER(data),
            .numFrames = ANIM_GET_NUM_FRAMES(data),
            .animFrame = 0,
            .accumTicks = delay,
            .delay = delay,
            .currentFrame = ANIM_GET_FIRST_FRAME(data)
    };
}

void SetAnimationDelay(AnimationState *animationState, int delay) {
    animationState->delay = delay;
}

static inline void updateAnimationState(AnimationState * animationState) {
    animationState->accumTicks--;
    if (animationState->accumTicks <= 0) {
        animationState->animFrame++;
        animationState->animFrame %= animationState->numFrames;
        animationState->currentFrame = animationState->frames[animationState->animFrame];
        animationState->accumTicks = animationState->delay;
    }
}

void HGL_ANIM_updateAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(animationStatePool, AnimationState)
        updateAnimationState(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

void HGL_ANIM_deleteAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(animationStatePool, AnimationState)
    HGL_ANIM_delete(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}
