#include "hgl_text.h"
#include "../pool/ObjectPool.h"
#include "../engine/sprites.h"

#define MAX_HGL_TEXTS 40

static ObjectPool * textPool;

void HGL_TEXT_init() {
    textPool = new_ObjectPool(MAX_HGL_TEXTS, sizeof(HGL_Text));
}

//void HGL_TEXT_update() {}

static inline void HGL_TEXT_render(HGL_Text *text)
{
    draw_text8(text->file, text->base_map, text->text, text->x, text->y, text->flags, text->length);
}

void HGL_TEXT_renderAll()
{
    OBJECTPOOL_ITERATOR_ALLOCATED_START(textPool, HGL_Text)
        HGL_TEXT_render(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

HGL_Text* HGL_TEXT_new(int file, int base_map, const char* text, int x, int y, int flags, int length) {
    PoolElement * elem = ObjectPool_get(textPool);
    HGL_Text * hgltext = elem->data;

    *hgltext = (HGL_Text) {
        .file = file,
        .base_map = base_map,
        .x = x,
        .y = y,
        .text = text,
        .flags = flags,
        .length = length
    };

    return hgltext;
}

void HGL_TEXT_delete(HGL_Text * text) {
    ObjectPool_free(textPool, text);
}

void HGL_TEXT_deleteAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(textPool, HGL_Text)
    HGL_TEXT_delete(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}