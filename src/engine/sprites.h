#ifndef __HGL_SPRITES_H__
#define __HGL_SPRITES_H__

#include <stdint.h>
#include "particle.h"

#include "../core/hgl_types.h"

typedef struct{
      int x; 
      int y;
      int z;
      int ult_z;
      int file;
      int graph;
      //int size;
      int size_x;
      int size_y;
      int alpha;
      int angle;
      int flags;
      int id;
      int v00[3];
      int v01[3];
      int v10[3];
      int v11[3];
      int fast;
      int uploadTpage;
}Tsprite;

//extern SPRITE psxSpritePool[4096];

void initSprites();
Tsprite *new_sprite(int x, int y, int z,int file, int graph);
void delete_sprite(Tsprite * spr);
void draw_all_sprites_basic();
void draw_all_sprites_zorder();

void draw_particle(int file, int graph, Particle *p);
void draw_sprite(Tsprite * spr);
void draw_sprite_fast(Tsprite * spr);

void draw_tile16_fast(Tsprite * spr);
void draw_tile8_fast(Tsprite * spr);

void draw_text8(int file, int base_map, const char* text, int posX, int posY, int flags, int length);

#endif