#ifndef FADER_H
#define FADER_H

#include <stdbool.h>
#include "../core/hgl_types.h"

enum FadeColor { WhiteFadeColor = 1, BlackFadeColor = 2 };

typedef struct Fader Fader;
typedef struct Fader {
    bool faded;
    bool fading;
    int wait;
    int i;
    fun2 (fadeFunction, uint8_t);
    fun2 (fadeIn, Fader*);
    fun2 (fadeOut, Fader*);
    fun2 (update, Fader*);
}Fader;

void fadeOut();
void fadeIn();

void setFadeColor(enum FadeColor fadeColor);
static inline void setWhiteFadeColor() { setFadeColor(WhiteFadeColor); }
static inline void setBlackFadeColor() { setFadeColor(BlackFadeColor); }

static inline void blackFadeOut() { setBlackFadeColor(); fadeOut(); }
static inline void whiteFadeOut() { setWhiteFadeColor(); fadeOut(); }

void updateFader();
void initFader(Fader * fader);
bool isFaded();

#endif //FADER_H
