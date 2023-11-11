#include "sprites.h"

#include "../core/hgl_scroll.h"

#include "../engine/tilemap.h"
#include "../media/fpg.h"

#define MAX_SPRITES 1024

#define MAX_SCROLLS 10
HGL_Scroll * scrolls[MAX_SCROLLS];

Tsprite proceso[MAX_SPRITES];  //lista de procesos
Tsprite *lista_z[MAX_SPRITES];
int num_procesos = 0;
int new_proceso = 0;

void setScroll(int index, HGL_Scroll* scroll) {
    scrolls[index] = scroll;
}

void initSprites() {
     num_procesos = 0;
     new_proceso = 0;
     for (int i=0; i<MAX_SPRITES; i++)     
     {   
         proceso[i].id = -1;
     }
     for (int i=0; i<MAX_SCROLLS; i++)
     {
         scrolls[i] = NULL;
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
    if (new_proceso > num_procesos) {
        num_procesos++;
    }
    return nuevo;
}


void delete_sprite(Tsprite * spr) {
    int spriteId = spr->id;
    //printf("delete sprite %i %i\n", spriteId, num_procesos);
    if (spriteId != -1) {
        if(spriteId == num_procesos - 1 ) {
            num_procesos--;
        }
        new_proceso = MIN(spriteId, new_proceso);
        spr->id = -1;
    }
}

inline static void drawScroll(HGL_Scroll* scroll) {
    draw_tilemap_no_wrap(scroll->file, scroll->base_map, scroll->tilemap, scroll->offsetX, scroll->offsetY, scroll->flags);
}

inline static void drawSprite(Tsprite * sprite) {
    if(sprite->fast) {
        draw_sprite_fast(sprite);
    } else {
        draw_sprite(sprite);
    }
}

void draw_all_sprites_basic(){
    for (int i=0; i<MAX_SCROLLS; i++) {
        HGL_Scroll * scroll = scrolls[i];
        if (scroll == NULL) break;
        drawScroll(scroll);
    }

    for (int i = num_procesos-1; i >= 0; i--)     
    {
        drawSprite(&proceso[i]);
    }
}

void insertsort(Tsprite*data[], int n){
    Tsprite* tmp;
    int i,j;

    for(j=1; j<n; j++){
        i = j-1;
        tmp = data[j];
        while((i>=0 && (tmp->z > data[i]->z))){
            data[i+1]=data[i];
            i--;
        }
        data[i+1]=tmp;
    }
}

void draw_all_sprites_zorder(){
    int n=0;
    for (int i=0; n < num_procesos; i++) {
        lista_z[n] = &proceso[i];
        n++;
    }
    insertsort(lista_z, num_procesos);
    for (int i = 0; i < num_procesos; i++) {
        //printf("%i,", sprite->z);
        drawSprite(lista_z[i]);
    }
    //printf(" -> %i\n", num_procesos);
}

enum DrawableType { DrawableNode, DrawableSprite, DrawableTileMap };

typedef struct {
    enum DrawableType type;
    void * drawable;
    void * next;
}Drawable;

#define ORDER_TABLE_LENGTH 8

Drawable drawable_buffer[MAX_SPRITES + 10];
static Drawable ot[ORDER_TABLE_LENGTH];
Drawable * nextDrawable = &drawable_buffer[0];
Drawable * headDrawable;

void init_draw_layers(Drawable *ot, int length) {
    nextDrawable = drawable_buffer;
    for (int i = 1; i<length; i++) {
        ot[i].type = DrawableNode;
        ot[i].next = &ot[i-1];
    }
    ot[0].type = DrawableNode;
    ot[0].next = NULL;
    headDrawable = &ot[length-1];
}

void init_draw_layersR(Drawable *ot, int length) {
    nextDrawable = drawable_buffer;
    for (int i = 0; i<length - 1; i++) {
        ot[i].type = DrawableNode;
        ot[i].next = &ot[i+1];
    }
    ot[length-1].type = DrawableNode;
    ot[length-1].next = NULL;
    headDrawable = &ot[0];
}


void add_sprite(Drawable *ot, Tsprite * sprite) {
    nextDrawable->type = DrawableSprite;
    nextDrawable->drawable = sprite;
    nextDrawable->next = ot->next;
    ot->next = nextDrawable;
    nextDrawable++;
}

void add_scroll(Drawable *ot, HGL_Scroll * scroll) {
    nextDrawable->type = DrawableTileMap;
    nextDrawable->drawable = scroll;
    nextDrawable->next = ot->next;
    ot->next = nextDrawable;
    nextDrawable++;
}

static void addAllScrolls() {
    for (int i=0; i<MAX_SCROLLS; i++) {
        HGL_Scroll * scroll = scrolls[i];
        if (scroll == NULL) return;
        add_scroll(&ot[scroll->z], scroll);
    }
}
static void addAllSprites() {
    for (int i = num_procesos-1; i >= 0; i--)
    {
        Tsprite *sprite = &proceso[i];
        add_sprite(&ot[sprite->z], sprite);
    }
}

static void buildDrawableList() {
    init_draw_layers(ot, ORDER_TABLE_LENGTH);
    addAllScrolls();
    addAllSprites();
}

void draw_all_sprites_layer() {
    buildDrawableList();
    const Drawable * next = headDrawable;
    Tsprite *sprite;
    HGL_Scroll *scroll;
    while(next != NULL)
    {
        switch (next->type) {
            case DrawableSprite:
                drawSprite(next->drawable);
                break;
            case DrawableTileMap:
                drawScroll(next->drawable);
                break;
            default:
                break;;
        }
        next = next->next;
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

    int numCols = tilemap->numCols;
    int numRows = tilemap->numRows;

    //destination rectangle
    // NES RESOLUTION 256x224
    /*int destX = 32;
    int maxDestX = 256 + destX;
    int destY = 8;
    int maxDestY = 224 + destY;*/

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

    int posXBase = destX - offsetX;
    int posYBase = destY - offsetY;

    tilemapSprite.z = 7;
    tilemapSprite.file = file;
    tilemapSprite.uploadTpage = 0;
    tilemapSprite.fast = 1;

    int drawTileWithIndex0 = flags;
    const uint8_t *tilePtr = tilemap->map + initialX + initialY * numCols;
    int lineIncrement = numCols - (finalX - initialX);
    //printf("lineIncrement %i\n", lineIncrement);
    for(int y = initialY; y < finalY; y++) {
        for(int x = initialX; x < finalX; x++) {
            tilemapSprite.y = posYBase + (y << 3);
            int tileIndex = *tilePtr;
            if (drawTileWithIndex0 || tileIndex > 0) {
                tilemapSprite.x = posXBase + (x << 3);
                tilemapSprite.graph = tileIndex + base_map - 1; //-1 because of tiled base_map is used to get the SPRITE later
//printf("%i ", tilemapSprite.graph);
                //This could be optimized using SPRT_8 and SPRT_16
                //draw_sprite_fast(&tilemapSprite);
                //draw_tile16_fast(&tilemapSprite); //SPRT_16
                draw_tile8_fast(&tilemapSprite); //SPRT_8
            }
            tilePtr++;
        }
        tilePtr += lineIncrement;
    }
    //printf("\n");
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

#include <string.h>

void draw_text8(int file, int base_map, const char* text, int posX, int posY, int flags, int length) {
    int offsetY = -posY;
    int offsetX = -posX;

    int numCols = length;
    if (length<0) numCols = strlen(text);

    int numRows = 1;

    //destination rectangle
    // NES RESOLUTION 256x224
    /*int destX = 32;
    int maxDestX = 256 + destX;
    int destY = 8;
    int maxDestY = 224 + destY;*/

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

    int posXBase = destX - offsetX;
    int posYBase = destY - offsetY;

    tilemapSprite.z = 7;
    tilemapSprite.file = file;
    tilemapSprite.uploadTpage = 0;
    tilemapSprite.fast = 1;

    int drawTileWithIndex0 = flags;
    const char *tilePtr = text + initialX + initialY * numCols;
    int lineIncrement = numCols - (finalX - initialX);

    for(int y = initialY; y < finalY; y++) {
        for(int x = initialX; x < finalX; x++) {
            tilemapSprite.y = posYBase + (y << 3);
            int tileIndex = *tilePtr - 31;
            if (drawTileWithIndex0 || tileIndex > 0) {
                tilemapSprite.x = posXBase + (x << 3);
                tilemapSprite.graph = tileIndex + base_map - 1; //-1 because of tiled base_map is used to get the SPRITE later
                //This could be optimized using SPRT_8 and SPRT_16
                //draw_sprite_fast(&tilemapSprite);
                //draw_tile16_fast(&tilemapSprite); //SPRT_16
                draw_tile8_fast(&tilemapSprite); //SPRT_8
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