#include "../../core/hgl_types.h"
//#include "ctoy_fpg.h"
#include "../fpg.h"
#include "../../engine/sprites.h"
#include "../../pool/FixedPool.h"
//#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <stdio.h>	// Not necessary but include it anyway

void CopySprite(SPRITE *original, SPRITE *sprite) {
    sprite->m_image.data = original->m_image.data;
    sprite->m_image.size = original->m_image.size;
    sprite->m_image.width = original->m_image.width;
    sprite->m_image.height = original->m_image.height;
    sprite->m_image.comp = original->m_image.comp;
    sprite->m_image.type = original->m_image.type;

    sprite->w = original->w;
    sprite->h = original->h;
    sprite->u = original->u;
    sprite->v = original->v;
    //sprite->col.r = original->col.r;
    //sprite->col.g = original->col.g;
    //sprite->col.b = original->col.b;
}
