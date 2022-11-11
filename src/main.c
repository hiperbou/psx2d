#include "hgl.h"
#include "system.h"
#include "input.h"
#include "sprites.h"
#include "fpg.h"

#include "game/sonic.h"
#include "cppfunction.h"
#include "hgl_types.h"

#include "game/actors.h"
#include "hgl_anim.h"
#include "game/camera.h"

#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <stdio.h>	// Not necessary but include it anyway
#include <libetc.h>	// Includes some functions that controls the display
#include <libgte.h>	// GTE header, not really used but libgpu.h depends on it
#include <libgpu.h>	// GPU library header

//#include "art/texture64.h"
//#include "art/girl01.h"
//#include "art/girl02.h"

#include <stdint.h>
#include <stdbool.h>

//#include "smb3map.h"
//#include "smb3ma2.h"
#include "smb3scene.h"
#include "smb3col.h"

void updatePhysic(Tsprite * spr/*HGL_Entity *ent*/, u16 input);


//#include "ghz1.h"
#include "ghz16.h"
//int bgaMap[] = { 0,0,0,0,0,0,1,2,3,4,0,0,0,0,0,0,1,2,3,4,0,0,0,0,0,0,1,2,3,4,0,0,0,0,0,0,1,2,3,4,0,0,0,0,0,5,6,7,8,9,0,0,0,0,0,5,6,7,8,9,0,0,0,0,0,5,6,7,8,9,0,0,0,0,0,5,6,7,8,9,0,0,0,0,0,10,11,12,13,14,0,1,2,3,4,10,11,12,13,14,0,0,0,0,0,10,11,12,13,14,0,1,2,3,4,10,11,12,13,14,0,0,0,0,0,0,15,16,17,18,5,6,7,8,9,0,15,16,17,18,0,0,0,0,0,0,15,16,17,18,5,6,7,8,9,0,15,16,17,18,0,0,0,0,0,0,19,20,21,0,10,11,12,13,14,0,19,20,21,0,0,0,0,0,0,0,19,20,21,0,10,11,12,13,14,0,19,20,21,0,0,0,0,0,0,0,0,20,21,0,0,15,16,17,18,0,0,20,21,0,0,0,0,0,0,0,0,20,21,0,0,15,16,17,18,0,0,20,21,0,0,0,0,0,0,0,0,20,21,0,0,19,20,21,0,0,0,20,21,0,0,0,0,0,0,0,0,20,21,0,0,19,20,21,0,0,0,20,21,0,0,0,0,0,22,23,24,20,21,0,0,0,20,21,0,0,0,20,21,0,0,0,0,0,22,23,24,20,21,0,0,0,20,21,0,0,0,20,21,0,0,0,0,0,25,26,27,20,21,0,0,0,20,21,0,0,0,20,21,0,0,0,0,0,25,26,27,20,21,0,0,0,20,21,0,0,0,20,21,0,0,0,0,0,28,29,30,20,31,32,33,32,34,21,0,0,0,20,21,0,0,0,0,0,28,29,30,20,31,32,33,32,34,21,0,0,0,20,21,0,0,0,0,0,28,35,30,20,36,37,38,37,39,21,0,0,0,20,21,0,0,0,0,0,28,35,30,20,36,37,38,37,39,21,0,0,0,20,21,0,40,41,42,40,43,44,30,20,45,46,47,46,48,49,40,40,40,50,49,40,40,41,42,40,43,44,30,20,45,46,47,46,48,49,40,40,40,50,49,40,51,52,53,51,54,55,56,57,58,59,60,61,62,63,51,51,51,64,63,51,51,52,53,51,54,55,56,57,58,59,60,61,62,63,51,51,51,64,63,51,65,65,65,65,65,66,67,68,69,70,71,72,73,65,65,65,65,65,65,65,65,65,65,65,65,66,67,68,69,70,71,72,73,65,65,65,65,65,65,65 };
//int bgaNumCols = 40;
//int bgaNumRows = 14;


int bgbMap[] = { 0,1,2,3,4,5,0,0,0,0,1,2,3,4,5,0,6,7,8,9,0,1,2,3,4,5,0,0,0,0,1,2,3,4,5,0,6,7,8,9,0,0,10,11,0,12,0,0,0,0,0,13,14,15,16,0,0,0,0,0,0,0,10,11,0,12,0,0,0,0,0,13,14,15,16,0,0,0,0,0,0,0,0,0,0,0,0,0,17,18,0,19,20,21,22,0,0,0,0,0,0,0,0,0,0,0,0,0,17,18,0,19,20,21,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,0,0,0,0,0,0,0,0,23,0,0,0,0,23,0,0,0,0,24,25,23,0,0,0,0,0,0,0,23,0,0,0,0,23,0,0,0,0,24,25,23,0,0,0,0,26,27,28,29,30,31,32,33,29,34,35,36,28,37,38,39,40,34,41,42,26,27,28,29,30,31,32,33,29,34,35,36,28,37,38,39,40,34,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,80,80,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,80,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,98,98,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,98,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,143,150,148,151,152,153,154,155,156,139,140,141,142,143,144,145,146,147,148,149,143,150,148,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,166,168,169,170,162,171,172,173,174,157,158,159,160,161,162,163,164,165,166,167,166,168,169,170,162,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,179,180,189,190,191,179,175,176,177,178,179,180,181,182,183,184,185,186,187,188,179,180,189,190,191,179 };
int bgbNumCols = 40;
int bgbNumRows = 14;

extern   int active_particles;
extern   int maxIterations;
/*
typedef struct {
    uint8_t version;
    uint8_t bigEndian;
    uint8_t stringEncoding;
    uint8_t orientation;
    uint8_t staggerAxis;
    uint8_t staggerIndex;
    uint16_t hexSideLength;
    uint16_t mapWidth;
    uint16_t mapHeight;
    uint16_t tileWidth;
    uint16_t tileHeight;
    uint8_t tileNumberBytes;
    uint8_t rle;
    uint16_t layerCount;
    uint8_t columnMajor;
    uint8_t objectNames;
    uint8_t objectTypes;
} __attribute__ ((packed)) TiledHeader;

typedef struct {
    uint32_t byteCount;
    uint8_t tilesetCount;
} __attribute__ ((packed)) TiledTilesetsHeader;

typedef struct {
    uint8_t firstTileID; //Type is defined by TileHeader.tileNumberBytes
    uint8_t name[7]; // it depends on the name XD "tiles" + 14 + 0 ??
    uint8_t filename[16]; // it depends on the name XD "smb3-tiles.png" + 14 + 0 ??
    uint16_t tileWidth;
    uint16_t tileHeight;
    uint16_t tileCount;
    uint16_t columnCount;
} __attribute__ ((packed)) TiledTilesetData;

typedef struct {
    uint32_t byteCount;
    uint16_t animationCount;
    //uint8_t animationCount2; //padding because byteCount is 2
    //uint8_t animationCount3; //padding because byteCount is 2
} __attribute__ ((packed)) TiledAnimationsHeader;

typedef struct {
    uint32_t byteCount;
    uint8_t tileLayerIdent;
    char name[7]; //"layer" + 14 + 0?
    uint16_t numberOfProperties;
    uint8_t typeLayer;
    uint8_t data;
} __attribute__ ((packed)) TiledLayerHeader;

typedef struct {
    TiledHeader header;
    TiledTilesetsHeader tilesetsHeader;
    TiledTilesetData tilesetData; //this could be an array
    TiledAnimationsHeader animationsHeader;
    TiledLayerHeader layerHeader;
} __attribute__ ((packed)) TiledBin;

void printTiledLayerHeader(TiledLayerHeader layerHeader) {
    printf("byteCount %i\n", layerHeader.byteCount);
    printf("tileLayerIdent %i\n", layerHeader.tileLayerIdent);
    printf("name %s\n", layerHeader.name);
    printf("numberOfProperties %i\n", layerHeader.numberOfProperties);
    printf("typeLayer %i\n", layerHeader.typeLayer);
}

void printTiledAnimationsHeader(TiledAnimationsHeader animationsHeader) {
    printf("byteCount %i\n", animationsHeader.byteCount);
    printf("animationCount %i\n", animationsHeader.animationCount);
}

void printTiledTilesetData(TiledTilesetData tilesetData) {
    printf("firstTileID %i\n", tilesetData.firstTileID);
    printf("name %s\n", tilesetData.name);
    printf("filename %s\n", tilesetData.filename);
    printf("tileWidth %i\n", tilesetData.tileWidth);
    printf("tileHeight %i\n", tilesetData.tileHeight);
    printf("tileCount %i\n", tilesetData.tileCount);
    printf("columnCount %i\n", tilesetData.columnCount);
}


void printTiledTilesetsHeaderInfo(TiledTilesetsHeader tilesetsHeader) {
    printf("byteCount %i\n", tilesetsHeader.byteCount);
    printf("tilesetCount %i\n", tilesetsHeader.tilesetCount);
}

void printTiledHeaderInfo(TiledHeader header) {
    printf("Version %i\n", header.version);
    printf("bigEndian %i\n", header.bigEndian);
    printf("stringEncoding %i\n", header.stringEncoding);
    printf("orientation %i\n", header.orientation);
    printf("staggerAxis %i\n", header.staggerAxis);
    printf("staggerIndex %i\n", header.staggerIndex);
    printf("hexSideLength %i\n", header.hexSideLength);
    printf("mapWidth %i\n", header.mapWidth);
    printf("mapHeight %i\n", header.mapHeight);
    printf("tileWidth %i\n", header.tileWidth);
    printf("tileHeight %i\n", header.tileHeight);
    printf("tileNumberBytes %i\n", header.tileNumberBytes);
    printf("rle %i\n", header.rle);
    printf("layerCount %i\n", header.layerCount);
    printf("columnMajor %i\n", header.columnMajor);
    printf("objectNames %i\n", header.objectNames);
    printf("objectTypes %i\n", header.objectTypes);
}*/
/*
void printMapInfo(TiledBin* tmx) {
    printTiledHeaderInfo(tmx->header);
    printTiledTilesetsHeaderInfo(tmx->tilesetsHeader);
    printTiledTilesetData(tmx->tilesetData);
    printTiledAnimationsHeader(tmx->animationsHeader);
    printTiledLayerHeader(tmx->layerHeader);

    //printf("sizeof TiledHeader %i should be 23\n", sizeof(TiledHeader));
    //printf("sizeof TiledLayerHeader %i\n", sizeof(TiledLayerHeader));
}*/
/*
TileMap fromTiledBin(TiledBin*tmx) {
    return (TileMap) {
        .map = &tmx->layerHeader.data,
        .numCols = tmx->header.mapWidth,
        .numRows = tmx->header.mapHeight
    };
}*/
/*
TileMap fromTiledBinOpt(TiledBin*tmx) {

    uint8_t *ptr = tmx;//&tmx->tilesetsHeader;
    //printf("tilesetsHeader.byteCount %i\n", tmx->tilesetsHeader.byteCount);
    ptr += 23; //header
    //printf("tilesetsHeader header guess\n");
    //printTiledTilesetsHeaderInfo(*(TiledTilesetsHeader *)ptr);
    ptr += ((TiledTilesetsHeader *)ptr)->byteCount + 4;

    TiledAnimationsHeader * animationsHeader = ptr;
    //printf("Animation header guess\n");
    //printTiledAnimationsHeader(*animationsHeader);
    //printf("Animation header guess END\n");
    ptr += animationsHeader->byteCount + 4;
    //printf("TiledLayerHeader guess\n");
    //TiledLayerHeader * layerHeader = ptr;
    //printTiledLayerHeader(*layerHeader);
    //printf("TiledLayerHeader guess END\n");

    //ptr = &layerHeader->name;
    ptr += 4;  // uint32_t byteCount;
    ptr += 1;  // uint8_t tileLayerIdent;
    ptr += 2; // Weird string prefix or padding?
    while(*ptr!=0) {
        //printf("name %c %i\n", *ptr,*ptr);
        ptr++;
    }
    ptr += 2; //uint16_t numberOfProperties;
    ptr += 1; //uint8_t typeLayer;
    //ptr  should be uint8_t data;

    return (TileMap) {
            .map = ptr,//&tmx->layerHeader.data,
            .numCols = tmx->header.mapWidth,
            .numRows = tmx->header.mapHeight
    };
}*/

//Amiga AMOS Pro Tome (.map) and Turbo Plus (.scene)
TileMap fromTiledBinScene(const uint8_t *tmx, int padding) {
    const uint8_t *ptr = tmx;

    uint8_t l = *ptr;
    uint8_t r = *(ptr + 1);
    int width = (l<<1) | r; //Stored in Big-endian
    ptr+=2;
    l = *ptr;
    r = *(ptr + 1);
    int height = (l<<1) | r; //Stored in Big-endian
    ptr += padding;    //Undocumented header, contains layer name "layer" or "collision"

    return (TileMap) {
            .map = ptr,
            .numCols = width,
            .numRows = height
    };
}

void wait(u_char *message) {
    int wait = 20;
    while(wait) {
        FntPrint(message);
        wait--;
        frame();
    }
}

#define TILE_SIZE 16
#define HALF_TILE_SIZE (TILE_SIZE/2)
#define TILE(X) FIX32(X*TILE_SIZE)
#define TILE_X_TO_SCREEN(X) ((X*TILE_SIZE) - camposx)
#define TILE_CENTER_X_TO_SCREEN(X) ((X*TILE_SIZE) + HALF_TILE_SIZE - camposx)
#define TILE_Y_TO_SCREEN(X) ((X*TILE_SIZE) - camposy)
#define TILE_CENTER_Y_TO_SCREEN(X) ((X*TILE_SIZE) + HALF_TILE_SIZE - camposy)
#define POS_TO_TILE_16(X) (fix32ToInt(X)>>4)

#define REPEAT5(X) X; X; X; X; X;
#define REPEAT10(X) REPEAT5(X); REPEAT5(X);
#define REPEAT25(X) REPEAT10(X); REPEAT10(X); REPEAT5(X);
#define REPEAT50(X) REPEAT25(X); REPEAT25(X);
#define REPEAT100(X) REPEAT50(X); REPEAT50(X);


void onPlayerCollidedWithCeilingTile(PlayerEventHandler*playerEventHandler, Tile tile) {
    switch (tile.id) {
        case 0: break;
        case 1:
            setTileAt(playerEventHandler->collisionTilemap, tile.tileX, tile.tileY, 0);
            setTileAt(playerEventHandler->tilemap, tile.tileX, tile.tileY, 1);
            int x = TILE_CENTER_X_TO_SCREEN(tile.tileX);
            int y = TILE_CENTER_Y_TO_SCREEN(tile.tileY);
            REPEAT25(new_Particle(x, y))
            break;
        default:
            break;
    }
}

bool onPlayerCollidedWithFloorTile(PlayerEventHandler*playerEventHandler, Tile tile) {
    uint8_t tileId = *getTileAt(playerEventHandler->tilemap, tile.tileX, tile.tileY);
    switch (tileId) {
        case 0: break;
        case 42:
        case 77:
            playerEventHandler->player->sonic.doRebound();

            setTileAt(playerEventHandler->collisionTilemap, tile.tileX, tile.tileY, 0);
            setTileAt(playerEventHandler->tilemap, tile.tileX, tile.tileY, 1);
            int x = TILE_CENTER_X_TO_SCREEN(tile.tileX);
            int y = TILE_CENTER_Y_TO_SCREEN(tile.tileY);
            REPEAT25(new_Particle(x, y))
            return true;
            break;
        default:
            break;
    }
    return false;
}

void initPlayerEventHandler(PlayerEventHandler*playerEventHandler, TileMap *tilemap, TileMap* collisionTilemap) {
    playerEventHandler->collisionTilemap = collisionTilemap;
    playerEventHandler->tilemap = tilemap;
    playerEventHandler->onColidedWithCeilingTile = onPlayerCollidedWithCeilingTile;
    playerEventHandler->onColidedWithFloorTile = onPlayerCollidedWithFloorTile;
    //playerEventHandler->player = player;
}


void checkCoin(TileMap* tileMap, Actor * actor) {
    int actorHalfWidth = FIX32(6);
    int actorHalfHeight = FIX32(6);
    int actorBoundingBoxY = FIX32(16);
    int minTileX = POS_TO_TILE_16(actor->entity->x - actorHalfWidth);
    int maxTileX = POS_TO_TILE_16(actor->entity->x + actorHalfWidth);
    int minTileY = POS_TO_TILE_16(actor->entity->y - actorBoundingBoxY);
    int maxTileY = POS_TO_TILE_16(actor->entity->y + actorHalfHeight);

    for (int tileY = minTileY; tileY <= maxTileY; tileY++) {
        for (int tileX = minTileX; tileX <= maxTileX; tileX++) {
            uint8_t* coinTile = getTileAt(tileMap, tileX, tileY);
            if (*coinTile == 7) {
                *coinTile = 1;
                REPEAT5(new_Particle(TILE_CENTER_X_TO_SCREEN(tileX), TILE_CENTER_Y_TO_SCREEN(tileY)))
            }
        }
    }
}

int main() {
    printf("Hello world demo\n");
    //printMapInfo(&smb3ma2);

    int result = someCppFunction(1,2);
    printf("result from cpp %i\n", result);

    init();

    HGL_ANIM_init();
    HGL_SPR_init();
    HGL_ENT_init();
    HGL_ACTOR_init();
    
    int system_fpg = 0;
    //int girl_fpg = new_fpg();
    int sonic_fpg = new_fpg();
    int tiles_fpg = new_fpg();
    int enemies_fpg = new_fpg();
    //int tiles2_fpg = new_fpg();

    //int texture64_map = load_map_from_memory(system_fpg, texture64);
    //int girl01_map = load_map_from_memory(girl_fpg, girl01);
    //int girl02_map = load_map_from_memory(girl_fpg, girl02);

    int texture64_map = load_map(system_fpg, "art/texture6");
    printf("texture64_map %i\n", texture64_map);
    //int girl01_map = load_map(girl_fpg, "art/girl01");
    //int girl02_map = load_map(girl_fpg, "art/girl02");

    int sonic_map = load_atlas(sonic_fpg, "art/sonic", 48, 48, 5, 5);

    ////int bga_map = load_atlas(tiles_fpg, "art/bga", 16, 16, 10, 8);
    ////int bga_map = load_atlas(tiles_fpg, "art/ghz1tile", 8, 8, 26, 25);
    //int bga_map = load_atlas(tiles_fpg, "art/ghz16", 16, 16, 16, 15);
    int bga_map = load_atlas(tiles_fpg, "art/smb3t", 16, 16, 11, 9);

    int bgb_map = load_atlas(tiles_fpg, "art/bgb", 16, 16, 15, 13);
    int enemies_map = load_atlas(enemies_fpg, "art/enemies", 48, 32, 4, 2);
    /*TileMap bgbTilemap;
    bgbTilemap.map = bgbMap;
    bgbTilemap.numCols = bgbNumCols;
    bgbTilemap.numRows = bgbNumRows;  */

    /*TileMap bgaTilemap;
    bgaTilemap.map = bgaMap;
    bgaTilemap.numCols = bgaNumCols;
    bgaTilemap.numRows = bgaNumRows;*/

    ///TileMap bgaTilemap = fromTiledBinOpt(&smb3ma2);
    TileMap bgaTilemap = fromTiledBinScene(smb3scene,28);
    TileMap collisionTilemap = fromTiledBinScene(smb3col,32);


    //Tsprite* spriteQuad = new_sprite(32, 32, 0, system_fpg, texture64_map);
    //Tsprite* spriteQuad2 = new_sprite(128, 128, 0, system_fpg, texture64_map);
    //Tsprite* spriteQuad3 = new_sprite(256, 256, 0, system_fpg, texture64_map);
    //Tsprite* spriteGirl = new_sprite(64, 64, 0, girl_fpg, girl01_map);
    Tsprite* spriteGirl = new_sprite(64, 64, 0, sonic_fpg, sonic_map);
    
    int x = 0;
    int y = 0;

    int matri = 0;

    int bgbx = 0;
    int bgby = 0;

    fix32 mx = FIX32(128);
    fix32 my = FIX32(180);

    PlayerEventHandler playerEventHandler;
    initPlayerEventHandler(&playerEventHandler, &bgaTilemap, &collisionTilemap);

    Actor * sonic = newSonic(sonic_fpg, FIX32(128), FIX32(128), collisionTilemap, &playerEventHandler);
    playerEventHandler.player = sonic;


    newMotobug(enemies_fpg, TILE(20),TILE(25));
    newMotobug(enemies_fpg, TILE(64),TILE(24));
    newMotobug (enemies_fpg, TILE(90), TILE(25));
    newBee(enemies_fpg, TILE(130),TILE(19));

    Actor * camera = newCamera(sonic, FIX32(40), FIX32(128));

    int cooldown = 0;
    while(1)
    {
        FntPrint("HELLO WORLD!\n");

        unsigned short btn = getButtons(0);
        if(btn & PAD_UP)
        {
            //bgby++;
            //y--;
            spriteGirl->flags = spriteGirl->flags | 2;
        }
        else if(btn & PAD_DOWN)
        {
            //bgby--;
            //y++;
            spriteGirl->flags = spriteGirl->flags & ~2;
        }
        if(btn & PAD_LEFT)
        {
            //printf("leftttt\n");
            //bgbx++;
            //x--;
            spriteGirl->flags = spriteGirl->flags | 1;
        }
        else if(btn & PAD_RIGHT)
        {
            //printf("righttt\n");
            //bgbx--;
            //x++;
            spriteGirl->flags = spriteGirl->flags & ~1;
        }

        if(btn & PAD_CROSS)
        {
            //spriteGirl->file = system_fpg;
        }
        if(btn & PAD_SQUARE)
        {
            //spriteGirl->file = girl_fpg;
            if (cooldown==0) {
                newMotobug(enemies_fpg, FIX32(128 + 64), my);
                cooldown = 30;
            }
            cooldown--;
        }

        int extraRotation = x << 4;
        int extraSize = x << 3;

        //spriteGirl->x = 32 + x;
        //spriteGirl->y = 32 + y;
        spriteGirl->angle = extraRotation;
        spriteGirl->size_x = 4096 + extraSize;

        matri++;
        matri = matri % (6<<0);
        if(matri==0) {
            spriteGirl->graph++;
            if(spriteGirl->graph>22) {
                spriteGirl->graph = 1;
            }
            //new_Particle(spriteGirl->x, spriteGirl->y);
            //printf("active particles %i iterations %i\n", active_particles, maxIterations);
        }
        //new_Particle(spriteGirl->x, spriteGirl->y);

        sonic->sonic.handleInput(btn);
        //updatePhysic(spriteGirl/*HGL_Entity *ent*/, btn);

        HGL_ANIM_updateAll();

        bgbx = camposx;
        bgby = camposy;

        HGL_ENT_updateAll();
        HGL_ACTOR_updateAll();

        //Handle collisions
        checkCoin(&bgaTilemap, sonic);


        HGL_ENT_renderAll(bgbx,bgby);
        HGL_SPR_renderAll();

        draw_all_sprites_basic();

        update_Particles();

        //addPrimitive(x + 32,32,0,64,64,255,255,0);
        //addPrimitive(x + 64,64,0,64,64,255,0,255);
        //addPrimitive(x + 96,96,0,64,64,0,255,255);


        //draw_tilemap_no_wrap(tiles_fpg, 1, &collisionTilemap, bgbx, bgby, 0); //Front
        draw_tilemap_no_wrap(tiles_fpg, 1, &bgaTilemap, bgbx, bgby, 0); //Front

        //draw_tilemap(tiles_fpg, bgb_map, &bgbTilemap, bgbx>>1, bgby>>1, 1); //BK
        //draw_tilemap_no_wrap(tiles_fpg, bgb_map, &bgbTilemap, 0, 0, 1); //BK

        frame();
    }

    return 0;
}
