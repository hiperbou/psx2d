#ifndef DEMO_HGL_ANIM_H
#define DEMO_HGL_ANIM_H

typedef struct {
    int* data;
    int* frames;
    int numFrames;
    int accumTicks;
    int speed;
    int animFrame;
    int currentFrame;
}AnimationState;

void HGL_ANIM_init();
AnimationState* HGL_ANIM_new();
void HGL_ANIM_delete(AnimationState *e);
void HGL_ANIM_updateAll();

void SetAnimationState(AnimationState * animationState, const int* data, int speed);
//void updateAnimationState(AnimationState * animationState);

#endif //DEMO_HGL_ANIM_H
