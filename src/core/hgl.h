#ifndef __HGL_H__
#define __HGL_H__

#include <stdint.h>

int HGL_init(); // Modified to return int status
void HGL_frame();
void startFrame(); // Added for explicit call from main loop
void HGL_quit();   // Added for explicit call from main

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

#endif