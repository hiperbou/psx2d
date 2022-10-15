#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <stdio.h>	// Not necessary but include it anyway
#include <libetc.h>	// Includes some functions that controls the display
#include <libgte.h>	// GTE header, not really used but libgpu.h depends on it
#include <libgpu.h>	// GPU library header
#include <libcd.h>

extern int db;

typedef struct _SPRITE {
    u_short tpage;  // Tpage value
    u_short clut;   // CLUT value
    u_char  u,v;    // UV offset (useful for non page aligned TIMs)
    u_char  w,h;    // Size (primitives can only draw 256x256 anyway)
    CVECTOR col;
} SPRITE;

void initNTSCDisplay();
void initPALDisplay();
void initClearColor();
void applyEnvironments();
void initDisplay();
void initCD();
void initMemory();
void display();
void initFont();
void LoadTexture(u_long * data, TIM_IMAGE * timImage);
void LoadTextureFromDisc(char *filename, TIM_IMAGE * timImage);
void GetSprite(TIM_IMAGE *tim, SPRITE *sprite);
void GetSpriteFromDisc(char *filename, SPRITE *sprite);
void GetSpriteFromMemory(u_char *data, SPRITE *sprite);
void drawTexts();
void clearOrderingTable();
void addPrimitive(int x, int y, int w, int h, int r, int g, int b);
void addSpriteTim(int x, int y,  int w, int h, int tim_uoffs, int tim_voffs, RECT tim_crect);
void addSprite(int x, int y, SPRITE *sprite, int uploadTpage);
void addRotSprite( int x, int y, int angle, int scale, int flags, SPRITE *sprite, int uploadTpage);
void addTile16(int x, int y, SPRITE *sprite);
void addTile8(int x, int y, SPRITE *sprite);
void addTexturePage(int tim_mode, RECT tim_prect);
void setTexturePage(int tim_mode, RECT tim_prect, RECT tim_crect);
void sortTpage(u_short newTpage);
u_long *loadfile(char *filename);

#endif