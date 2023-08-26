#include "../core/hgl_types.h"
#include "../core/hgl_file.h"

#include "fpg.h"
#include "fpgInternal.h"
#include "../engine/sprites.h"
#include "../pool/FixedPool.h"

#include <stdint.h>

div_fpg *fpg[MAX_NUM_FPGS];

FixedPool *fpgPool;
FixedPool *mapPool;

FixedPool *psxSpritePool;

static void init_pools() {
    fpgPool = new_FixedPool(MAX_NUM_FPGS, sizeof(div_fpg));
    mapPool = new_FixedPool(MAX_NUM_MAPS, sizeof(div_map));

    psxSpritePool = new_FixedPool(MAX_NUM_MAPS, sizeof(SPRITE));
}

int num_fpgs=0;
int libre_fpg=0;

int new_fpg() {
    fpg[libre_fpg] = FixedPool_get(fpgPool);
    fpg[libre_fpg]->total_mapas=0;
    //fpg[libre_fpg]->texture_size=0;
    fpg[libre_fpg]->mapa_libre=1;
    
    int mi_fpg=libre_fpg;
    int n;
    for (n=0; n<MAX_NUM_MAPS; n++){
        fpg[libre_fpg]->map[n]=NULL;
    }    
    
    for (n=0; n<MAX_NUM_FPGS; n++){
        if(fpg[n]==NULL){
            break;
        }    
    }
    libre_fpg = n;

    num_fpgs++;
    return mi_fpg;
}


int init_fpgs() {
    init_pools();
    libre_fpg=0;
    for (int n=0; n<MAX_NUM_FPGS; n++){
        fpg[n]=NULL;
    }    
    return new_fpg(); //hace el fpg de sistema para cargar graficos y cosas de esas
}

int new_map(int file,int graph) {
    fpg[file]->map[graph] = FixedPool_get(mapPool);
    fpg[file]->map[graph]->image = FixedPool_get(psxSpritePool);
    //fpg[file]->map[graph]->center_x=(unsigned short *)malloc(2);
    //fpg[file]->map[graph]->center_y=(unsigned short *)malloc(2);
    //fpg[file]->map[graph]->center_x[0]=0;
    //fpg[file]->map[graph]->center_y[0]=0;
    //fpg[file]->map[graph]->num_points=1;
    //fpg[file]->map[graph]->w=0;
    //fpg[file]->map[graph]->h=0;
    //fpg[file]->map[graph]->flag=0;
    //fpg[file]->map[graph]->imagegl=0;
    //fpg[file]->map[graph]->tv=0;
    //fpg[file]->map[graph]->th=0;
    //fpg[file]->map[graph]->tile_text=NULL;
    //fpg[file]->map[graph]->tsize=0;

    //fpg[file]->map[graph]->posx=0; //pos inicial para texturas de opengl
    //fpg[file]->map[graph]->posy=0;

    fpg[file]->total_mapas++;
    if(fpg[file]->map[graph+1]==NULL){
        fpg[file]->mapa_libre=graph+1;
    }

    return graph;
}

int get_free_map(int file) {
    if (fpg[file] == NULL) return 0;
    if (fpg[file]->map[fpg[file]->mapa_libre] == NULL) return fpg[file]->mapa_libre;
    for (int i=1; i<MAX_NUM_MAPS; i++) {
        if (fpg[file]->map[i] == NULL) {
            return i;
        }
    }   
    return 0;
}



int load_atlas(int file, char *filename, int tileWidth, int tileHeight, int numCols, int numRows) {
    int atlas_map;
    int map;
    SPRITE * atlas_sprite = NULL;
    SPRITE * sprite = NULL;

    //printf("load_atlas\n");

    for (int y=0; y < numRows; y++) {
        for(int x=0; x < numCols; x++) {
            if (atlas_sprite == NULL) {
                //printf("load_map %i %s %i %i\n", file, filename,x,y);
                atlas_map = load_map(file, filename);
                atlas_sprite = fpg[file]->map[atlas_map]->image;
                sprite = atlas_sprite;
            } else {
                //printf("get_free_map %i x %i y %i\n", file, x, y);
                map = new_map(file, get_free_map(file));
                sprite = fpg[file]->map[map]->image;
                CopySprite(atlas_sprite, sprite);
            }

            sprite->u = x * tileWidth;
            sprite->v = y * tileHeight;
            sprite->w = tileWidth;
            sprite->h = tileHeight;
        }
    }
    return atlas_map;
}


int load_map(int file, char *filename) {
    int mapa = 1;
    if (fpg[file]->map[fpg[file]->mapa_libre] == NULL) {
        mapa = fpg[file]->mapa_libre;
    } else {
        mapa = get_free_map(file);
    }
    if (mapa > 0) {
        new_map(file,mapa);
        GetSpriteFromDisc(filename, fpg[file]->map[mapa]->image);

        //load_gl_texture2(name,file,mapa);
        //if(CENTER_LOADED_IMAGES==1){
        //    fpg[file]->map[mapa]->center_x[0]=fpg[file]->map[mapa]->w/2;
        //    fpg[file]->map[mapa]->center_y[0]=fpg[file]->map[mapa]->h/2;
        //}
        //fpg[0]->map[mapa]->image=NULL;
        return mapa;
    } else {
        printf("DEMASIADOS MAPAS EN EL FPG, cree uno nuevo y use load_map(int file,char name)!!\n");
        return 0;
    }
}

int load_map_from_memory(int file, uint8_t *data){
    int mapa = 1;
    if (fpg[file]->map[fpg[file]->mapa_libre] == NULL) {
        mapa = fpg[file]->mapa_libre;
    } else {
        mapa = get_free_map(file);
    }
    if (mapa > 0) {
        new_map(file,mapa);
        GetSpriteFromMemory(data, fpg[file]->map[mapa]->image);

        //load_gl_texture2(name,file,mapa);
        //if(CENTER_LOADED_IMAGES==1){
        //    fpg[file]->map[mapa]->center_x[0]=fpg[file]->map[mapa]->w/2;
        //    fpg[file]->map[mapa]->center_y[0]=fpg[file]->map[mapa]->h/2;
        //}
        //fpg[0]->map[mapa]->image=NULL;
        return mapa;
    } else {
        printf("DEMASIADOS MAPAS EN EL FPG, cree uno nuevo y use load_map(int file,char name)!!\n");
        return 0;
    }
}