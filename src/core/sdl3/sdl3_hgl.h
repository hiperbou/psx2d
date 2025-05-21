#ifndef SDL3_HGL_H
#define SDL3_HGL_H

#include <SDL3/SDL.h>
#include <stdint.h> // For uint8_t

// Core graphics functions
void HGL_init(void);
void HGL_frame(void);
void HGL_close(void); // For cleanup

void setClearColor(uint8_t r, uint8_t g, uint8_t b);
void fadeToBlack(uint8_t fade);
void fadeToWhite(uint8_t fade);

// Accessor for the global SDL_Renderer
SDL_Renderer* HGL_GetRenderer(void);

#endif // SDL3_HGL_H
