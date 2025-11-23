#ifndef __HGL_H__
#define __HGL_H__

#include <stdint.h>

void HGL_init();
void HGL_frame();

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
}ClearColor;

void setClearColor(uint8_t r, uint8_t g, uint8_t b);

inline void setClearColor2(const ClearColor * clearColor) {
    setClearColor(clearColor->r, clearColor->g, clearColor->b);
}

void fadeToBlack(uint8_t fade);
void fadeToWhite(uint8_t fade);

typedef void MainLoopCallback(void);
void setMainLoopCallback(MainLoopCallback *mainLoop);

double HGL_getTime();

#endif