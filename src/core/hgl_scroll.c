#include "hgl_scroll.h"

#include "../engine/tilemap.h"

#include "../pool/ObjectPool.h"
#include "../engine/sprites.h"

#define MAX_HGL_SCROLL 8

static ObjectPool * scrollPool;

void HGL_SCROLL_init() {
    scrollPool = new_ObjectPool(MAX_HGL_SCROLL, sizeof(HGL_Scroll));
}

//void HGL_Scroll_update() {}


static inline void HGL_SCROLL_render(const HGL_Scroll *scroll)
{
    setScroll(0, scroll);
    //draw_tilemap_no_wrap(scroll->file, scroll->base_map, scroll->tilemap, scroll->offsetX, scroll->offsetY, scroll->flags);
}

void HGL_SCROLL_renderAll()
{
    OBJECTPOOL_ITERATOR_ALLOCATED_START(scrollPool, HGL_Scroll)
        HGL_SCROLL_render(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

HGL_Scroll* HGL_SCROLL_new(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int z, int flags) {
    const PoolElement * elem = ObjectPool_get(scrollPool);
    HGL_Scroll * hglscroll = elem->data;

    *hglscroll = (HGL_Scroll) {
        .file = file,
        .base_map = base_map,
        .tilemap = tilemap,
        .offsetX = offsetX,
        .offsetY = offsetY,
        .z = z,
        .flags = flags
    };

    return hglscroll;
}

void HGL_SCROLL_delete(HGL_Scroll * scroll) {
    ObjectPool_free(scrollPool, scroll);
}

void HGL_SCROLL_deleteAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(scrollPool, HGL_Scroll)
    HGL_SCROLL_delete(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}