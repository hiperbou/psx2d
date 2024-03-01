#ifndef HGL_SCROLL_H_
#define HGL_SCROLL_H_


#include "hgl_types.h"
#include "../engine/tilemap.h"

typedef struct{
    int file;
    int base_map;
    TileMap *tilemap;
    int offsetX;
    int offsetY;
    int z;
    int flags;
}HGL_Scroll;

void HGL_SCROLL_init();
//void HGL_TEXT_update();

void HGL_SCROLL_renderAll();

HGL_Scroll* HGL_SCROLL_new(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int z, int flags);

void HGL_SCROLL_delete(HGL_Scroll * scroll);
void HGL_SCROLL_deleteAll();

//inline void HGL_SCROLL_setOffset(HGL_Scroll*scroll, int offsetX, int offsetY) { scroll->offsetX = offsetX; scroll->offsetY = offsetY; }
void HGL_SCROLL_setOffset(HGL_Scroll*scroll, int offsetX, int offsetY);

#endif /* HGL_SCROLL_H_ */
