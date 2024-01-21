#include "../core/hgl.h"
#include "../core/hgl_anim.h"
#include "fader.h"
#include <stdbool.h>
#include <stdio.h>

ANIM(FadeOutAnim, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210, 225, 240, 255)
ANIM(FadeInAnim, 255, 240, 225, 210, 195, 180, 165, 150, 135, 120, 105, 90, 75, 60, 45, 30, 15)

static void noop(Fader *fader) {}

void fadeOutRoutine(Fader *fader) {
    fader->fadeFunction(ANIM_GET_FRAME(FadeOutAnim, fader->i));
    if(!fader->fading) return;

    if(fader->i < fader->wait) {
        fader->i++;
    } else {
        fader->fading = false;
    }
}

void fadeInRoutine(Fader *fader) {
    if(!fader->fading) return;

    fader->fadeFunction(ANIM_GET_FRAME(FadeInAnim, fader->i));

    fader->i++;
    if(fader->i > fader->wait) {
        fader->faded = false;
        fader->fading = false;
        fader->update = noop;
    }
}

static Fader fader = (Fader) {
    .faded =  false,
    .fading =  false,
    .wait = 0,
    .i = 0,
    .update =  noop,
    .fadeFunction = fadeToBlack,
    .fadeIn = fadeInRoutine,
    .fadeOut = fadeOutRoutine
};

void initFader(Fader * fader) {
    *fader = (Fader) {
        .faded =  false,
        .fading =  false,
        .wait = 0,
        .i = 0,
        .fadeFunction = fadeToBlack,
        .update =  noop,
        .fadeIn = noop,
        .fadeOut = noop
    };
}

void startFadeOut(Fader *fader) {
    fader->faded = true;
    fader->fading = true;
    fader->wait = ANIM_GET_NUM_FRAMES(FadeOutAnim) - 1;
    fader->i = 0;
    fader->update = fader->fadeOut;
}

void startFadeIn(Fader *fader) {
    fader->faded = true;
    fader->fading = true;
    fader->wait = ANIM_GET_NUM_FRAMES(FadeInAnim) - 1;
    fader->i = 0;
    fader->update = fader->fadeIn;
}

void fadeOut() {
    startFadeOut(&fader);
}

void fadeIn() {
    startFadeIn(&fader);
}

void updateFader() {
    fader.update(&fader);
}

void setFadeColor(enum FadeColor fadeColor) {
    switch (fadeColor) {
        case WhiteFadeColor:
            fader.fadeFunction = fadeToWhite;
        break;
        case BlackFadeColor:
            fader.fadeFunction = fadeToBlack;
        default:
            fader.fadeFunction = fadeToBlack;
    }
}

bool isFaded() {
    return fader.faded;
}