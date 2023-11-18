#ifndef FADER_H
#define FADER_H

#include <stdbool.h>
#include "../core/hgl_types.h"

enum FadeColor { WhiteFadeColor = 1, BlackFadeColor = 2 };

typedef struct Fader Fader;
typedef struct Fader {
    bool fading;
    int wait;
    int i;
    fun2 (fadeFunction, uint8_t);
    fun2 (fadeIn, Fader*);
    fun2 (fadeOut, Fader*);
    fun2 (update, Fader*);
}Fader;

void fadeOff();
void fadeOn();
void drawFader();

void setFadeColor(enum FadeColor fadeColor);
inline void setWhiteFadeColor() { setFadeColor(WhiteFadeColor); }
inline void setBlackFadeColor() { setFadeColor(BlackFadeColor); }

void initFader(Fader * fader);

#endif //FADER_H
