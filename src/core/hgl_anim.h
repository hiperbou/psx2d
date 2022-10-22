//
// Created by DaniW on 22/10/2022.
//

#ifndef DEMO_HGL_ANIM_H
#define DEMO_HGL_ANIM_H

typedef struct {
    int* anim_data;
    int* anim_frames;
    int anim_numFrames;
    int anim_accum_ticks;
    int anim_speed;
    int anim_frame;
    int currentFrame;
}AnimationState;

void HGL_ANIM_init();
AnimationState* HGL_ANIM_new();
void HGL_ANIM_delete(AnimationState *e);
void HGL_ANIM_updateAll();

void SetAnimationState(AnimationState * animationState, const int* data, int speed);
//void updateAnimationState(AnimationState * animationState);

#endif //DEMO_HGL_ANIM_H
