#include "sprites.h"
#include "../engine/tilemap.h"
#include "../media/fpg.h"

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
        if(spriteId==num_procesos -1 ) {
            num_procesos--;
        }
        new_proceso = MIN(spriteId, new_proceso);
        spr->id = -1;
    }
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
    const uint8_t *tileMap = tilemap->map;
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

static Tsprite tilemapSprite;


static inline int mod_floor(int a, int n) {
    return ((a % n) + n) % n;
}

#define MOD_FLOOR(A, N) (((A % N) + N) % N)

void draw_tilemap_no_wrap(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags) {

    int numCols = tilemap->numCols;
    int numRows = tilemap->numRows;

    //destination rectangle
    // NES RESOLUTION 256x224
    /*int destX = 32;
    int maxDestX = 256 + destX;
    int destY = 8;
    int maxDestY = 224 + destY;*/

    // FULL SCREEN NTSC
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

    int posXBase = destX - offsetX;
    int posYBase = destY - offsetY;

    tilemapSprite.z = 6;
    tilemapSprite.file = file;
    tilemapSprite.uploadTpage = 0;
    tilemapSprite.fast = 1;

    int drawTileWithIndex0 = flags;

    const uint8_t *tilePtr = tilemap->map + initialX + initialY * numCols;
    int lineIncrement = numCols - (finalX - initialX);
    for(int y = initialY; y < finalY; y++) {
        for(int x = initialX; x < finalX; x++) {
            tilemapSprite.y = posYBase + (y << 4);
            int tileIndex = *tilePtr;
            if (drawTileWithIndex0 || tileIndex > 0) {
                tilemapSprite.x = posXBase + (x << 4);
                tilemapSprite.graph = tileIndex + base_map - 1; //-1 because of tiled base_map is used to get the SPRITE later

                //This could be optimized using SPRT_8 and SPRT_16
                //draw_sprite_fast(&tilemapSprite);
                draw_tile16_fast(&tilemapSprite); //SPRT_16
                //draw_tile8_fast(&tilemapSprite); //SPRT_8
            }
            tilePtr++;
        }
        tilePtr += lineIncrement;
    }

#ifdef PSX
    SPRITE* sprite = fpg[file]->map[base_map]->image;
    sortTpage(sprite->tpage, tilemapSprite.z);
#endif
}

void draw_tilemap_no_wrap8(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags) {
    const uint8_t *tileMap = tilemap->map;
    int numCols = tilemap->numCols;
    int numRows = tilemap->numRows;

    //destination rectangle
    int destX = 0;
    int maxDestX = 320;
    int destY = 0;
    int maxDestY = 240;

    int minX = offsetX >> 3;
    int maxX = (((maxDestX - destX) + offsetX) >> 3) + 1;

    int initialX = CLAMP(minX, 0, numCols);
    int finalX = CLAMP(maxX, 0, numCols);

    int minY = offsetY >> 3;
    int maxY = (((maxDestY - destY) + offsetY) >> 3) + 1;

    int initialY = CLAMP(minY, 0, numRows);
    int finalY = CLAMP(maxY, 0, numRows);

    tilemapSprite.z = 7;
    tilemapSprite.file = file;
    tilemapSprite.uploadTpage = 0;
    tilemapSprite.fast = 1;

    int drawTileWithIndex0 = flags;
    static int step = 60;
    int counter = 0;

    int posXBase = destX - offsetX;
    int posYBase = destY - offsetY;
    int posYTile = 0;
    int yMultipliedByCols = 0;
    int tileIndex = 0;
    for(int y = initialY; y < finalY; y++) {
        posYTile = (y << 3);
        yMultipliedByCols = y * numCols;
        for(int x = initialX; x < finalX; x++) {
            tileIndex = tileMap[x + yMultipliedByCols];
            if (drawTileWithIndex0 || tileIndex > 0) {
                tilemapSprite.x = posXBase + (x << 3);
                tilemapSprite.y = posYBase + posYTile;
                tilemapSprite.graph =  tileIndex + base_map;

                //This could be optimized using SPRT_8 and SPRT_16
                //draw_sprite_fast(&tilemapSprite);
                //draw_tile16_fast(&tilemapSprite); //SPRT_16
                draw_tile8_fast(&tilemapSprite); //SPRT_8
                counter++;
            }
        }
    }

    step--;
    if(step==0) {
        step = 60;
        printf("tiles drawn %i\n", counter);
    }
#ifdef PSX
    SPRITE* sprite = fpg[file]->map[base_map]->image;
    sortTpage(sprite->tpage, tilemapSprite.z);
#endif
}

void draw_tilemap(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags) {
    const uint8_t *tileMap = tilemap->map;
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
#ifdef PSX
    SPRITE* sprite = fpg[file]->map[base_map]->image;
    sortTpage(sprite->tpage, tilemapSprite.z);
#endif
}

void draw_tilemap8(int file, int base_map, TileMap *tilemap, int offsetX, int offsetY, int flags) {
    const uint8_t *tileMap = tilemap->map;
    int numCols = tilemap->numCols;
    int numRows = tilemap->numRows;

    //destination rectangle
    int destX = 0;
    int maxDestX = 320;
    int destY = 0;
    int maxDestY = 240;

    int minX = offsetX >> 3;
    int maxX = (((maxDestX - destX) + offsetX) >> 3) + 1;

    int initialX = CLAMP(minX, 0, numCols);
    int finalX = CLAMP(maxX, 0, numCols);

    int minY = offsetY >> 3;
    int maxY = (((maxDestY - destY) + offsetY) >> 3) + 1;

    //int initialY = CLAMP(minY, 0, numRows);
    //int finalY = CLAMP(maxY, 0, numRows);

    tilemapSprite.z = 7;
    tilemapSprite.file = file;
    tilemapSprite.uploadTpage = 0;
    tilemapSprite.fast = 1;

    int drawTileWithIndex0 = flags;
    static int step = 60;
    int counter = 0;

    for(int y = minY; y < maxY; y++) {
        for(int x = minX; x < maxX; x++) {
            int index = ((mod_floor(x, numCols)) + (mod_floor(y, numRows)) * numCols);
            if (drawTileWithIndex0 || tileMap[index] > 0) {
                tilemapSprite.x = destX - offsetX + x * 8;
                tilemapSprite.y = destY - offsetY + y * 8;
                tilemapSprite.graph =  tileMap[index] + base_map;

                //This could be optimized using SPRT_8 and SPRT_16
                //draw_sprite_fast(&tilemapSprite);
                //draw_tile16_fast(&tilemapSprite); //SPRT_16
                draw_tile8_fast(&tilemapSprite); //SPRT_8
                counter++;
            }
        }
    }
    step--;
    if(step==0) {
        step = 60;
        printf("tiles drawn %i\n", counter);
    }
#ifdef PSX
    SPRITE* sprite = fpg[file]->map[base_map]->image;
    sortTpage(sprite->tpage, tilemapSprite.z);
#endif
}

void draw_particle(int file, int graph, Particle *p) {
    //int file = 2;
    int base_map = graph;
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
#ifdef PSX
    SPRITE* sprite = fpg[file]->map[base_map]->image;
    sortTpage(sprite->tpage, tilemapSprite.z);
#endif
}