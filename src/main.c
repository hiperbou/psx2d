#include "hgl.h"
#include "system.h"
#include "input.h"
#include "sprites.h"
#include "fpg.h"

#include "sonic.h"
#include "cppfunction.h"
#include "hgl_types.h"

#include "game/actors.h"
#include "hgl_anim.h"

#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <stdio.h>	// Not necessary but include it anyway
#include <libetc.h>	// Includes some functions that controls the display
#include <libgte.h>	// GTE header, not really used but libgpu.h depends on it
#include <libgpu.h>	// GPU library header

//#include "art/texture64.h"
//#include "art/girl01.h"
//#include "art/girl02.h"

void updatePhysic(Tsprite * spr/*HGL_Entity *ent*/, u16 input);

int bgaMap[] = { 0,0,0,0,0,0,1,2,3,4,0,0,0,0,0,0,1,2,3,4,0,0,0,0,0,0,1,2,3,4,0,0,0,0,0,0,1,2,3,4,0,0,0,0,0,5,6,7,8,9,0,0,0,0,0,5,6,7,8,9,0,0,0,0,0,5,6,7,8,9,0,0,0,0,0,5,6,7,8,9,0,0,0,0,0,10,11,12,13,14,0,1,2,3,4,10,11,12,13,14,0,0,0,0,0,10,11,12,13,14,0,1,2,3,4,10,11,12,13,14,0,0,0,0,0,0,15,16,17,18,5,6,7,8,9,0,15,16,17,18,0,0,0,0,0,0,15,16,17,18,5,6,7,8,9,0,15,16,17,18,0,0,0,0,0,0,19,20,21,0,10,11,12,13,14,0,19,20,21,0,0,0,0,0,0,0,19,20,21,0,10,11,12,13,14,0,19,20,21,0,0,0,0,0,0,0,0,20,21,0,0,15,16,17,18,0,0,20,21,0,0,0,0,0,0,0,0,20,21,0,0,15,16,17,18,0,0,20,21,0,0,0,0,0,0,0,0,20,21,0,0,19,20,21,0,0,0,20,21,0,0,0,0,0,0,0,0,20,21,0,0,19,20,21,0,0,0,20,21,0,0,0,0,0,22,23,24,20,21,0,0,0,20,21,0,0,0,20,21,0,0,0,0,0,22,23,24,20,21,0,0,0,20,21,0,0,0,20,21,0,0,0,0,0,25,26,27,20,21,0,0,0,20,21,0,0,0,20,21,0,0,0,0,0,25,26,27,20,21,0,0,0,20,21,0,0,0,20,21,0,0,0,0,0,28,29,30,20,31,32,33,32,34,21,0,0,0,20,21,0,0,0,0,0,28,29,30,20,31,32,33,32,34,21,0,0,0,20,21,0,0,0,0,0,28,35,30,20,36,37,38,37,39,21,0,0,0,20,21,0,0,0,0,0,28,35,30,20,36,37,38,37,39,21,0,0,0,20,21,0,40,41,42,40,43,44,30,20,45,46,47,46,48,49,40,40,40,50,49,40,40,41,42,40,43,44,30,20,45,46,47,46,48,49,40,40,40,50,49,40,51,52,53,51,54,55,56,57,58,59,60,61,62,63,51,51,51,64,63,51,51,52,53,51,54,55,56,57,58,59,60,61,62,63,51,51,51,64,63,51,65,65,65,65,65,66,67,68,69,70,71,72,73,65,65,65,65,65,65,65,65,65,65,65,65,66,67,68,69,70,71,72,73,65,65,65,65,65,65,65 };
int bgaNumCols = 40;
int bgaNumRows = 14;

int bgbMap[] = { 0,1,2,3,4,5,0,0,0,0,1,2,3,4,5,0,6,7,8,9,0,1,2,3,4,5,0,0,0,0,1,2,3,4,5,0,6,7,8,9,0,0,10,11,0,12,0,0,0,0,0,13,14,15,16,0,0,0,0,0,0,0,10,11,0,12,0,0,0,0,0,13,14,15,16,0,0,0,0,0,0,0,0,0,0,0,0,0,17,18,0,19,20,21,22,0,0,0,0,0,0,0,0,0,0,0,0,0,17,18,0,19,20,21,22,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,23,0,0,0,0,0,0,0,0,23,0,0,0,0,23,0,0,0,0,24,25,23,0,0,0,0,0,0,0,23,0,0,0,0,23,0,0,0,0,24,25,23,0,0,0,0,26,27,28,29,30,31,32,33,29,34,35,36,28,37,38,39,40,34,41,42,26,27,28,29,30,31,32,33,29,34,35,36,28,37,38,39,40,34,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,80,80,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,80,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,98,98,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,98,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,143,150,148,151,152,153,154,155,156,139,140,141,142,143,144,145,146,147,148,149,143,150,148,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,166,168,169,170,162,171,172,173,174,157,158,159,160,161,162,163,164,165,166,167,166,168,169,170,162,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,179,180,189,190,191,179,175,176,177,178,179,180,181,182,183,184,185,186,187,188,179,180,189,190,191,179 };
int bgbNumCols = 40;
int bgbNumRows = 14;

extern   int active_particles;
extern   int maxIterations;

int main() {
    printf("Hello world demo\n");

    int result = someCppFunction(1,2);
    printf("result from cpp %i\n", result);

    init();

    HGL_ANIM_init();
    HGL_SPR_init();
    HGL_ENT_init();
    HGL_ACTOR_init();
    
    int system_fpg = 0;
    int girl_fpg = new_fpg();
    int sonic_fpg = new_fpg();
    int tiles_fpg = new_fpg();
    int enemies_fpg = new_fpg();
    //int tiles2_fpg = new_fpg();

    //int texture64_map = load_map_from_memory(system_fpg, texture64);
    //int girl01_map = load_map_from_memory(girl_fpg, girl01);
    //int girl02_map = load_map_from_memory(girl_fpg, girl02);

    int texture64_map = load_map(system_fpg, "art/texture6");
    int girl01_map = load_map(girl_fpg, "art/girl01");
    int girl02_map = load_map(girl_fpg, "art/girl02");

    int sonic_map = load_atlas(sonic_fpg, "art/sonic", 48, 48, 5, 5);
    int bga_map = load_atlas(tiles_fpg, "art/bga", 16, 16, 10, 8);
    int bgb_map = load_atlas(tiles_fpg, "art/bgb", 16, 16, 15, 13);

    int enemies_map = load_atlas(enemies_fpg, "art/enemies", 48, 32, 4, 2);

    TileMap bgbTilemap;
    bgbTilemap.map = bgbMap;
    bgbTilemap.numCols = bgbNumCols;
    bgbTilemap.numRows = bgbNumRows;   

    TileMap bgaTilemap;
    bgaTilemap.map = bgaMap;
    bgaTilemap.numCols = bgaNumCols;
    bgaTilemap.numRows = bgaNumRows;
    
    Tsprite* spriteQuad = new_sprite(32, 32, 0, system_fpg, texture64_map);
    Tsprite* spriteQuad2 = new_sprite(128, 128, 0, system_fpg, texture64_map);
    Tsprite* spriteQuad3 = new_sprite(256, 256, 0, system_fpg, texture64_map);
    //Tsprite* spriteGirl = new_sprite(64, 64, 0, girl_fpg, girl01_map);
    Tsprite* spriteGirl = new_sprite(64, 64, 0, sonic_fpg, sonic_map);
    
    int x = 0;
    int y = 0;

    int matri = 0;

    int bgbx = 0;
    int bgby = 0;


    fix32 mx = FIX32(128);
    fix32 my = FIX32(180);
    newMotobug(enemies_fpg, mx,my);
    newMotobug(enemies_fpg, FIX32(128+64),my);

    newBee(enemies_fpg, FIX32(128+64),FIX32(64));


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
        new_Particle(spriteGirl->x, spriteGirl->y);

        updatePhysic(spriteGirl/*HGL_Entity *ent*/, btn);

        HGL_ANIM_updateAll();

        HGL_ENT_updateAll(bgbx,bgby);

        HGL_ACTOR_updateAll();

        HGL_ENT_renderAll(bgbx,bgby);
        HGL_SPR_renderAll();

        draw_all_sprites_basic();

        update_Particles();

        addPrimitive(x + 32,32,64,64,255,255,0);
        addPrimitive(x + 64,64,64,64,255,0,255);
        addPrimitive(x + 96,96,64,64,0,255,255);

        draw_tilemap(tiles_fpg, 1, &bgaTilemap, bgbx*2, bgby*2, 0);
        draw_tilemap(tiles_fpg, bgb_map, &bgbTilemap, bgbx, bgby, 1);

        frame();
    }

    return 0;
}
