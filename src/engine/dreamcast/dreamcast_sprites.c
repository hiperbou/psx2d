#include "../../core/hgl.h"
#include "../../engine/sprites.h"
#include "../../media/fpg.h"
#include "../../media/dreamcast/dreamcast_fpg.h"
#include <kos.h>
#include <dc/pvr.h>

void draw_sprite(Tsprite * spr) {
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t vert;
    div_map *map = fpg[spr->file]->map[spr->graph];
    SPRITE *sprite = map->image;

    pvr_poly_cxt_txr(&cxt, PVR_LIST_OP_POLY, PVR_TXRFMT_RGB565 | PVR_TXRFMT_NONTWIDDLED, sprite->w, sprite->h, sprite->texture, PVR_FILTER_BILINEAR);
    pvr_poly_compile(&hdr, &cxt);
    pvr_prim(&hdr, sizeof(hdr));

    vert.flags = PVR_CMD_VERTEX;
    vert.x = spr->x;
    vert.y = spr->y;
    vert.z = spr->z;
    vert.u = 0.0f;
    vert.v = 0.0f;
    vert.argb = 0xffffffff;
    vert.oargb = 0;
    pvr_prim(&vert, sizeof(vert));

    vert.y += sprite->h;
    vert.v = 1.0f;
    pvr_prim(&vert, sizeof(vert));

    vert.x += sprite->w;
    vert.y -= sprite->h;
    vert.u = 1.0f;
    vert.v = 0.0f;
    pvr_prim(&vert, sizeof(vert));

    vert.flags = PVR_CMD_VERTEX_EOL;
    vert.y += sprite->h;
    vert.v = 1.0f;
    pvr_prim(&vert, sizeof(vert));
}

void draw_sprite_fast(Tsprite * spr) {
    draw_sprite(spr);
}

void draw_tile16_fast(Tsprite * spr) {
    draw_sprite(spr);
}

void draw_tile8_fast(Tsprite * spr) {
    draw_sprite(spr);
}
