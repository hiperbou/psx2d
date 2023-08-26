#include "../../core/hgl.h"
#include "system.h"
#include "input.h"
#include "../sprites.h"
#include "../../engine/tilemap.h"
#include "../../media/fpg.h"
#include "../../engine/particle.h"
#include "../../core/hgl_types.h"

#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <libetc.h>	// Includes some functions that controls the display
#include <libgte.h>	// GTE header, not really used but libgpu.h depends on it
#include <libgpu.h>	// GPU library header


void draw_sprite(Tsprite * spr) {
    if (spr->graph <= 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    addRotSprite(spr->x, spr->y, spr->z, spr->angle, spr->size_x, spr->flags, sprite, spr->uploadTpage);
}

void draw_sprite_fast(Tsprite * spr) {
    if (spr->graph < 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    addSprite(spr->x, spr->y, spr->z, sprite, spr->uploadTpage);
}

void draw_tile16_fast(Tsprite * spr) {
    if (spr->graph < 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    addTile16(spr->x, spr->y, spr->z, sprite);
}

void draw_tile8_fast(Tsprite * spr) {
    if (spr->graph < 0 || spr->id < 0) return;
    SPRITE* sprite = fpg[spr->file]->map[spr->graph]->image;
    addTile8(spr->x, spr->y, spr->z, sprite);
}