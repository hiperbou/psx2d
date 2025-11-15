#include "../../media/fpg.h"
#include "dreamcast_fpg.h"
#include "../../core/hgl_file.h"
#include "../../core/hgl_mem.h"

#include <stdio.h>

#define FPG_HEADER_SIZE 12
#define FPG_MAP_HEADER_SIZE 12

typedef struct {
    char magic[4]; // "fpg"
    uint32_t num_maps;
} FpgHeader;

typedef struct {
    uint32_t id;
    uint32_t width;
    uint32_t height;
    uint8_t data[];
} MapData;

int load_map_from_memory(int file, uint8_t *data) {
    FpgHeader* header = (FpgHeader*)data;
    if (header->magic[0] != 'd' || header->magic[1] != 'i' || header->magic[2] != 'v') {
        return -1; // Invalid magic
    }

    div_fpg *f = fpg[file];
    f->mapa_libre = 0;
    f->total_mapas = header->num_maps;

    uint8_t* map_ptr = data + FPG_HEADER_SIZE;
    for (int i = 0; i < header->num_maps; i++) {
        MapData* map_data = (MapData*)map_ptr;
        div_map *m = hgl_malloc(sizeof(div_map));
        m->image = hgl_malloc(sizeof(SPRITE));
        m->image->w = map_data->width;
        m->image->h = map_data->height;
        m->image->texture = load_texture(map_data->data, map_data->width, map_data->height);
        f->map[i] = m;
        map_ptr += FPG_MAP_HEADER_SIZE + (map_data->width * map_data->height * 2);
    }
    return 0;
}
