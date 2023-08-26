#ifndef __HGL__TILEMAP_H
#define __HGL__TILEMAP_H

#include <stdint.h>
#include "../core/hgl_types.h"

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

void draw_tilemap_no_wrap(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags);
void draw_tilemap_no_wrap8(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags);
void draw_tilemap(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags);
void draw_tilemap8(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags);


TileMap fromTiledBinScene(const uint8_t *tmx, int padding);
TileMap cloneTileMap(TileMap * original);

#endif //__HGL__TILEMAP_H
