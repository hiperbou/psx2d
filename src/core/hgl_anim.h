#ifndef DEMO_HGL_ANIM_H
#define DEMO_HGL_ANIM_H

typedef struct {
    int* data;
    int* frames;
    int numFrames;
    int accumTicks;
    int delay;
    int animFrame;
    int currentFrame;
}AnimationState;

void HGL_ANIM_init();
AnimationState* HGL_ANIM_new();
void HGL_ANIM_delete(AnimationState *e);
void HGL_ANIM_updateAll();

void SetAnimationState(AnimationState * animationState, const int* data, int delay);
void SetAnimationDelay(AnimationState * animationState, int delay);
//void updateAnimationState(AnimationState * animationState);

#define LAST_ARGUMENT(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a31, a32, a33, a34, a35, a36, a37, a38, a39, a40, a41, a42, a43, a44, a45, a46, a47, a48, a49, a50, a51, a52, a53, a54, a55, a56, a57, a58, a59, a60, a61, a62, a63, a64, a65, a66, a67, a68, a69, a70, a71, a72, a73, a74, a75, a76, a77, a78, a79, a80, a81, a82, a83, a84, a85, a86, a87, a88, a89, a90, a91, a92, a93, a94, a95, a96, a97, a98, a99, a100, N, ...) N
#define COUNT_ARGUMENTS(...) LAST_ARGUMENT(dummy, ## __VA_ARGS__, 89,88,87,86,85,84,83,82,81,80,79,78,77,76,75,74,73,72,71,70,69,68,67,66,65,64,63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define ANIM(NAME, ...) \
static const int NAME[] = { COUNT_ARGUMENTS(__VA_ARGS__), ##__VA_ARGS__ };

#endif //DEMO_HGL_ANIM_H
