#ifndef __HGL_SPRITES_H__
#define __HGL_SPRITES_H__

#include <stdint.h>
#include "system.h"
#include "particle.h"

#include "hgl_types.h"

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

typedef struct {
    const uint8_t * map;
    //int *map;
    int numCols;
    int numRows;
}TileMap;

typedef struct {
    uint8_t id;
    int tileX;
    int tileY;
}Tile;

inline static uint8_t* getTileAt(TileMap* tileMap, int tileX, int tileY) {
    tileX = CLAMP(tileX, 0, tileMap->numCols - 1);
    tileY = CLAMP(tileY, 0, tileMap->numRows - 1);
    return (uint8_t*)&tileMap->map[tileX + tileY * tileMap->numCols];
}

inline static void setTileAt(TileMap* tileMap, int tileX, int tileY, int value) {
    *getTileAt(tileMap, tileX, tileY) = value;
}

inline static Tile getTileInfo(TileMap* tileMap, int tileX, int tileY) {
    tileX = CLAMP(tileX, 0, tileMap->numCols - 1);
    tileY = CLAMP(tileY, 0, tileMap->numRows - 1);
    return (Tile) {
        .id = tileMap->map[tileX + tileY * tileMap->numCols],
        .tileX = tileX,
        .tileY = tileY
    };
}

//extern SPRITE psxSpritePool[4096];

void initSprites();
Tsprite *new_sprite(int x, int y, int z,int file, int graph);
void delete_sprite(Tsprite * spr);
void draw_all_sprites_basic();
void draw_tilemap_no_wrap(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags);
void draw_tilemap_no_wrap8(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags);
void draw_tilemap(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags);
void draw_tilemap8(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags);

void draw_particle(Particle *p);
void draw_sprite(Tsprite * spr);
void draw_sprite_fast(Tsprite * spr);

#endif