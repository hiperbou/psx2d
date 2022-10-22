#include "hgl_anim.h"
#include "ObjectPool.h"

#define MAX_NUM_ANIMATIONS 64

static ObjectPool * animationStatePool;

void HGL_ANIM_init()
{
    animationStatePool = new_ObjectPool(MAX_NUM_ANIMATIONS, sizeof(AnimationState));
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
    if(animationState->anim_data == data) return;

    *animationState = (AnimationState) {
            .anim_data = (int*) data,
            .anim_frames = (int *) &data[1],
            .anim_numFrames = data[0],
            .anim_frame = 0,
            .anim_accum_ticks = 0,
            .anim_speed = speed,
            .currentFrame = data[1]
    };
}

static inline void updateAnimationState(AnimationState * animationState) {
    animationState->anim_accum_ticks += animationState->anim_speed;
    if (animationState->anim_accum_ticks > 100) {
        animationState->anim_frame = (animationState->anim_frame + 1) % animationState->anim_numFrames;
        animationState->currentFrame = animationState->anim_frames[animationState->anim_frame];
        animationState->anim_accum_ticks -= 100;
    }
}

void HGL_ANIM_updateAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(animationStatePool, AnimationState)
        updateAnimationState(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

