#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

#include <stdint.h>
#include <stddef.h>
#include "../../engine/sprites.h" // For Tsprite
#include "../../input/input.h"   // For PADSTATE

typedef unsigned int GLuint;

typedef struct SPRITE {
    GLuint texture_id;
    int u, v;
    int w, h;
    int tex_w, tex_h;
} SPRITE;

void HGL_init();
void HGL_frame();
void setMainLoopCallback(void (*mainLoop)());
void setClearColor(uint8_t r, uint8_t g, uint8_t b);
void fadeToBlack(uint8_t fade);
void fadeToWhite(uint8_t fade);


void GetSpriteFromDisc(char *filename, SPRITE *sprite);
//void GetSpriteFromMemory(uint8_t *data,/* int len,*/ SPRITE *sprite);
void CopySprite(SPRITE *original, SPRITE *sprite);

void initInput();
PADSTATE* getPadState(int player);
uint16_t getButtons(int player);

void initSprites();
void draw_sprite(Tsprite *spr);
void draw_sprite_fast(Tsprite *spr);
void draw_tile16_fast(Tsprite *spr);
void draw_tile8_fast(Tsprite *spr);
void draw_text8(int file, int base_map, const char* text, int posX, int posY, int flags, int length);

#endif // OPENGL_BACKEND_H
