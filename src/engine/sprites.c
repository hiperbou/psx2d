#include "hgl.h"
#include "system.h"
#include "input.h"
#include "sprites.h"
#include "fpg.h"
#include "particle.h"
#include "hgl_types.h"

#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <stdio.h>	// Not necessary but include it anyway
#include <libetc.h>	// Includes some functions that controls the display
#include <libgte.h>	// GTE header, not really used but libgpu.h depends on it
#include <libgpu.h>	// GPU library header

#define MAX_SPRITES 1024

Tsprite proceso[MAX_SPRITES];  //lista de procesos
Tsprite *lista_z[MAX_SPRITES];
int num_procesos = 0;
int new_proceso = 0;

void initSprites() {
     for (int i=0; i<MAX_SPRITES; i++)     
     {   
         proceso[i].id = -1;
     }
}

Tsprite *new_sprite(int x, int y, int z,int file, int graph) {
    if(new_proceso >= MAX_SPRITES) {
        printf("No se pueden dibujar más sprites!! Cambie MAX_SPRITES en options.h");
        return 0;                          
    }

    Tsprite* nuevo = &proceso[new_proceso];
    nuevo->graph=graph;
    nuevo->file=file;
    nuevo->x=x;
    nuevo->y=y;
    nuevo->z=z;
    nuevo->ult_z=z;

    nuevo->size_x=4096;
    nuevo->size_y=4096;
    nuevo->alpha=4096;
    nuevo->angle=0;
    nuevo->flags=0;

    nuevo->v00[0]=128;
    nuevo->v01[0]=128;
    nuevo->v10[0]=128;
    nuevo->v11[0]=128;
    
    nuevo->v00[1]=128;
    nuevo->v01[1]=128;
    nuevo->v10[1]=128;
    nuevo->v11[1]=128;
    
    nuevo->v00[2]=128;
    nuevo->v01[2]=128;
    nuevo->v10[2]=128;
    nuevo->v11[2]=128;

    nuevo->fast = 0;
    nuevo->uploadTpage = 1;

    nuevo->id = new_proceso;

    do{
        new_proceso++;
        if (new_proceso == MAX_SPRITES) break;
    } while (proceso[new_proceso].id!=-1);
    //UPDATE_Z_ORDER=1;
    num_procesos++;
    return nuevo;
}


void delete_sprite(Tsprite * spr) {
    int spriteId = spr->id;
    if (spriteId != -1) {
        if(spriteId==num_procesos) {
            num_procesos--;
        }
        new_proceso = MIN(spriteId, new_proceso);
        spr->id = -1;
    }
}

void draw_sprite(Tsprite * spr) {
    if (spr->graph < 0 || spr->id < 0) return;
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

void draw_all_sprites_basic(){
    Tsprite * sprite;
    for (int i = num_procesos-1; i >= 0; i--)     
    {   
        sprite = &proceso[i];
        if(sprite->fast) {
            draw_sprite_fast(sprite);
        } else {
            draw_sprite(sprite);
        }
    }
}

void draw_tilemap_with_sprites(int file, int base_map, TileMap *tilemap) {
    Tsprite * tileB;
    Tsprite * firstTileB = NULL;
    int *tileMap = tilemap->map;
    int numCols = tilemap->numCols;
    int numRows = tilemap->numRows;

    for(int y=0; y<numRows; y++) {
        for(int x=0; x<numCols; x++) {
            if (tileMap[x + y * numCols] > 0) {
                //Ths has an y offset that shouldn't be needed
                tileB = new_sprite(x * 16, 8 + y * 16, 0, file, tileMap[x + y * numCols] + base_map);
                tileB->fast = 1;
                tileB->uploadTpage = 0;

                if(firstTileB==NULL) {
                    firstTileB = tileB;
                    tileB->uploadTpage = 1;
                }
            }
        }    
    }
}

Tsprite tilemapSprite;


static inline int mod_floor(int a, int n) {
    return ((a % n) + n) % n;
}

#define MOD_FLOOR(A, N) (((A % N) + N) % N)

void draw_tilemap_no_wrap(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags) {
    int *tileMap = tilemap->map;
    int numCols = tilemap->numCols;
    int numRows = tilemap->numRows;

    //destination rectangle
    int destX = 0;
    int maxDestX = 320;
    int destY = 0;
    int maxDestY = 240;

    int minX = offsetX >> 4;
    int maxX = (((maxDestX - destX) + offsetX) >> 4) + 1;

    int initialX = CLAMP(minX, 0, numCols);
    int finalX = CLAMP(maxX, 0, numCols);

    int minY = offsetY >> 4;
    int maxY = (((maxDestY - destY) + offsetY) >> 4) + 1;

    int initialY = CLAMP(minY, 0, numRows);
    int finalY = CLAMP(maxY, 0, numRows);

    tilemapSprite.z = 7;
    tilemapSprite.file = file;
    tilemapSprite.uploadTpage = 0;
    tilemapSprite.fast = 1;

    //offsetY = offsetY + 8; //Ths has an y offset of 8 that shouldn't be needed?

    int drawTileWithIndex0 = flags;

    for(int y = initialY; y < finalY; y++) {
        for(int x = initialX; x < finalX; x++) {
            if (drawTileWithIndex0 || tileMap[x + y * numCols] > 0) {
                tilemapSprite.x = destX - offsetX + x * 16;
                tilemapSprite.y = destY - offsetY + y * 16;
                tilemapSprite.graph =  tileMap[x + y * numCols] + base_map;

                //This could be optimized using SPRT_8 and SPRT_16
                //draw_sprite_fast(&tilemapSprite);
                draw_tile16_fast(&tilemapSprite); //SPRT_16
                //draw_tile8_fast(&tilemapSprite); //SPRT_8
            }
        }    
    }

    SPRITE* sprite = fpg[file]->map[base_map]->image;
    sortTpage(sprite->tpage, tilemapSprite.z);
}

void draw_tilemap(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags) {
    int *tileMap = tilemap->map;
    int numCols = tilemap->numCols;
    int numRows = tilemap->numRows;

    //destination rectangle
    int destX = 0;
    int maxDestX = 320;
    int destY = 0;
    int maxDestY = 240;

    int minX = offsetX >> 4;
    int maxX = (((maxDestX - destX) + offsetX) >> 4) + 1;

    int initialX = CLAMP(minX, 0, numCols);
    int finalX = CLAMP(maxX, 0, numCols);

    int minY = offsetY >> 4;
    int maxY = (((maxDestY - destY) + offsetY) >> 4) + 1;

    //int initialY = CLAMP(minY, 0, numRows);
    //int finalY = CLAMP(maxY, 0, numRows);

    tilemapSprite.z = 7;
    tilemapSprite.file = file;
    tilemapSprite.uploadTpage = 0;
    tilemapSprite.fast = 1;

    //offsetY = offsetY + 8; //Ths has an y offset of 8 that shouldn't be needed?

    int drawTileWithIndex0 = flags;

    for(int y = minY; y < maxY; y++) {
        for(int x = minX; x < maxX; x++) {
            int index = ((mod_floor(x, numCols)) + (mod_floor(y, numRows)) * numCols);
            if (drawTileWithIndex0 || tileMap[index] > 0) {
                tilemapSprite.x = destX - offsetX + x * 16;
                tilemapSprite.y = destY - offsetY + y * 16;
                tilemapSprite.graph =  tileMap[index] + base_map;

                //This could be optimized using SPRT_8 and SPRT_16
                //draw_sprite_fast(&tilemapSprite);
                draw_tile16_fast(&tilemapSprite); //SPRT_16
                //draw_tile8_fast(&tilemapSprite); //SPRT_8
            }
        }    
    }

    SPRITE* sprite = fpg[file]->map[base_map]->image;
    sortTpage(sprite->tpage, tilemapSprite.z);
}

void draw_particle(Particle *p) {
    int file = 2;
    int base_map = 1;
    tilemapSprite.file = file;
    tilemapSprite.graph = base_map;
    tilemapSprite.x = p->pos.x >> 12;
    tilemapSprite.y = p->pos.y >> 12;
    tilemapSprite.angle = p->angle;
    tilemapSprite.size_x = p->size * 40;//96;
    tilemapSprite.size_y = tilemapSprite.size_x;
    tilemapSprite.z = 0;
    tilemapSprite.uploadTpage = 1;
    tilemapSprite.fast = 0;
    draw_sprite(&tilemapSprite);
    //draw_sprite_fast(&tilemapSprite);
    SPRITE* sprite = fpg[file]->map[base_map]->image;
    sortTpage(sprite->tpage, tilemapSprite.z);
}