#include "tilemap.h"
#include "../core/hgl_mem.h"

//Amiga AMOS Pro Tome (.map) and Turbo Plus (.scene)
TileMap fromTiledBinScene(const uint8_t *tmx, int padding) {
    const uint8_t *ptr = tmx;

    uint8_t l = *ptr;
    uint8_t r = *(ptr + 1);
    int width = (l<<1) | r; //Stored in Big-endian
    ptr+=2;
    l = *ptr;
    r = *(ptr + 1);
    int height = (l<<1) | r; //Stored in Big-endian
    ptr += padding;    //Undocumented header, contains layer name "layer" or "collision"

    return (TileMap) {
            .map = ptr,
            .numCols = width,
            .numRows = height
    };
}

TileMap cloneTileMap(TileMap * original) {
    size_t size = sizeof(uint8_t) * original->numRows * original->numCols;
    uint8_t * newMap =  HGL_malloc(size);
    HGL_memcpy(newMap, (uint8_t*)original->map, size);

    return (TileMap) {
            .map = newMap,
            .numCols = original->numCols,
            .numRows = original->numRows
    };
}



