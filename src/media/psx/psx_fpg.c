#include "../../core/hgl_types.h"
#include "../fpg.h"
#include "../../engine/sprites.h"
#include "../../pool/FixedPool.h"

#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <stdio.h>	// Not necessary but include it anyway
#include <libetc.h>	// Includes some functions that controls the display
#include <libgte.h>	// GTE header, not really used but libgpu.h depends on it
#include <libgpu.h>	// GPU library header


void CopySprite(SPRITE *original, SPRITE *sprite) {
    sprite->tpage = original->tpage;
    sprite->clut = original->clut;
    sprite->w = original->w;
    sprite->h = original->h;
    sprite->u = original->u;
    sprite->v = original->v ;
    sprite->col.r = original->col.r;
    sprite->col.g = original->col.g;
    sprite->col.b = original->col.b;
}

