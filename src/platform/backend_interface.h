#ifndef BACKEND_INTERFACE_H
#define BACKEND_INTERFACE_H

#include <stdint.h>
#include <stddef.h>
#include "../engine/sprites.h" // For Tsprite
#include "../input/input.h"   // For PADSTATE

// Forward declaration - SPRITE typedef is defined by each platform-specific backend
typedef struct SPRITE SPRITE;

// Initialization functions
void HGL_init();

// Main loop and rendering control
void HGL_frame();
void setMainLoopCallback(void (*mainLoop)());
void setClearColor(uint8_t r, uint8_t g, uint8_t b);
void fadeToBlack(uint8_t fade);
void fadeToWhite(uint8_t fade);

// Sprite loading and manipulation
void GetSpriteFromDisc(char *filename, SPRITE *sprite);
void GetSpriteFromMemory(uint8_t *data, SPRITE *sprite);
void CopySprite(SPRITE *original, SPRITE *sprite);

// Input functions
PADSTATE* getPadState(int player);
uint16_t getButtons(int player);

// Drawing functions
void draw_sprite(Tsprite *spr);
void draw_sprite_fast(Tsprite *spr);
void draw_tile16_fast(Tsprite *spr);
void draw_tile8_fast(Tsprite *spr);
void draw_text8(int file, int base_map, const char* text, int posX, int posY, int flags, int length);

#endif // BACKEND_INTERFACE_H
