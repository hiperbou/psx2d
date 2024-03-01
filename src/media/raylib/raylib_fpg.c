#include "../../core/hgl_types.h"
#include "../fpg.h"
#include "../../engine/sprites.h"
#include "../../pool/FixedPool.h"


void CopySprite(SPRITE *original, SPRITE *sprite) {
    sprite->m_image.id = original->m_image.id;
    sprite->m_image.width = original->m_image.width;
    sprite->m_image.height = original->m_image.height;
    sprite->m_image.mipmaps = original->m_image.mipmaps;
    sprite->m_image.format = original->m_image.format;

    sprite->w = original->w;
    sprite->h = original->h;
    sprite->u = original->u;
    sprite->v = original->v;
    //sprite->col.r = original->col.r;
    //sprite->col.g = original->col.g;
    //sprite->col.b = original->col.b;
}
