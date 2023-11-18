#include "../core/hgl.h"
#include "../core/hgl_anim.h"
#include "fader.h"
#include <stdbool.h>
#include <stdio.h>

ANIM(FadeOutAnim, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180, 195, 210, 225, 240, 255)
ANIM(FadeInAnim, 255, 240, 225, 210, 195, 180, 165, 150, 135, 120, 105, 90, 75, 60, 45, 30, 15)

static void noop(Fader *fader) {}

void fadeOffRoutine(Fader *fader) {
    fader->fadeFunction(ANIM_GET_FRAME(FadeOutAnim, fader->i));
    if(!fader->fading) return;

    if(fader->i < fader->wait) {
        fader->i++;
    } else {
        fader->fading = false;
    }
}

void fadeOnRoutine(Fader *fader) {
    if(!fader->fading) return;

    fader->fadeFunction(ANIM_GET_FRAME(FadeInAnim, fader->i));

    fader->i++;
    if(fader->i > fader->wait) {
        fader->fading = false;
        fader->update = noop;
    }
}

static Fader fader = (Fader) {
    .fading =  false,
    .wait = 0,
    .i = 0,
    .update =  noop,
    .fadeFunction = fadeToBlack,
    .fadeIn = fadeOnRoutine,
    .fadeOut = fadeOffRoutine
};

void initFader(Fader * fader) {
    *fader = (Fader) {
        .fading =  false,
        .wait = 0,
        .i = 0,
        .fadeFunction = fadeToBlack,
        .update =  noop,
        .fadeIn = noop,
        .fadeOut = noop
    };
}

void startFadeOff(Fader *fader) {
    fader->fading = true;
    fader->wait = ANIM_GET_NUM_FRAMES(FadeOutAnim) - 1;
    fader->i = 0;
    fader->update = fader->fadeOut;
}

void startFadeOn(Fader *fader) {
    fader->fading = true;
    fader->wait = ANIM_GET_NUM_FRAMES(FadeInAnim) - 1;
    fader->i = 0;
    fader->update = fader->fadeIn;
}

void fadeOff() {
    startFadeOff(&fader);
}

void fadeOn() {
    startFadeOn(&fader);
}

void drawFader() {
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