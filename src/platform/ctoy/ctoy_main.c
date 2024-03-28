#include <ctoy.h>

#include "../../../util/img_util.c"

#include "../../core/hgl_types.h"
#include "../../core/hgl_actor.h"
#include "../../core/hgl_anim.h"
#include "../../core/hgl_command.h"
#include "../../core/hgl_ent.h"
#include "../../core/hgl_scroll.h"
#include "../../core/hgl_script.h"
#include "../../core/hgl_spr.h"
#include "../../core/hgl_text.h"
#include "../../engine/sprites.h"
#include "../../engine/text.h"
#include "../../engine/tilemap.h"
#include "../../game/actors.h"
#include "../../game/camera.h"
#include "../../input/input.h"
#include "../../input/buttonstate.h"
#include "../../media/fpg.h"

#include "../../utils/picoro.h"
//#include "../../utils/picoro.c"
int resumable(coro c) {}
void *resume(coro c, void *arg) { return NULL; }
void *yield(void *arg) { return NULL; }
coro coroutine(void *function(void *arg)) { return NULL; } 

#include "../../gameresources.c"
#define fsmState gameFsmState
#include "../../gamemain.c"
#include "../../game.c"
#include "../../gametitle.c"
#include "../../gameending.c"
#undef fsmState
#include "../../core/hgl_actor.c"
#include "../../core/ctoy/hgl.c"
#include "../../core/hgl_anim.c"
#include "../../core/hgl_command.c"
#include "../../core/hgl_ent.c"
#include "../../core/hgl_pool.c"
#include "../../core/hgl_scroll.c"
#include "../../core/hgl_script.c"
#include "../../core/hgl_spr.c"
#include "../../core/hgl_text.c"
#include "../../core/default/hgl_mem.c"
#include "../../core/ctoy/hgl_file.c"
#include "../../game/camera.c"
#include "../../input/buttonstate.c"
#include "../../input/ctoy/input.c"
#include "../../media/ctoy/ctoy_fpg.c"
#include "../../media/fpg.c"

#include "../../engine/fader.c"
#include "../../engine/tilemap.c"
#include "../../engine/sprites.c"
#include "../../engine/text.c"
#define rand m_rand
#include "../../engine/particle.c"
#undef rand
#include "../../engine/ctoy/ctoy_sprites.c"
#include "../../pool/FixedPool.c"
#include "../../pool/ObjectPool.c"
#include "../../utils/utils.c"

#include "../../game/data/gamedata.c"
#include "../../game/menu/coursemenu.c"
#include "../../game/state/gamestate.c"

#define anim_patrol bee_anim_patrol
#define updatePhysics bee_updatePhysics
#define update bee_update
#define constructor bee_constructor
#include "../../game/bee.c"
#undef anim_patrol
#undef updatePhysics
#undef update
#undef constructor

#define anim_patrol motobug_anim_patrol
#define update motobug_update
#define constructor motobug_constructor
#include "../../game/motobug.c"
#undef anim_patrol
#undef update
#undef constructor

#define constructor coin_constructor
#define update coin_update
#include "../../game/block.c"
#undef constructor
#undef update

#define constructor fallToBackgroundScript_constructor
#define update fallToBackgroundScript_update
#include "../../game/fallToBackgroundScript.c"
#undef constructor
#undef update

#define constructor triggerScript_constructor
#define update triggerScript_update
#include "../../game/triggerScript.c"
#undef constructor
#undef update

#define constructor goal_constructor
#define update goal_update
#include "../../game/goal.c"
#undef constructor
#undef update

#define constructor menustar_constructor
#define destructor menustar_destructor
#define update menustar_update
#define Idle menustar_Idle
#define Activated menustar_Activated
#define stateIdle statemenustar_Idle
#define stateActivated statemenustar_Activated
#include "../../game/menustar.c"
#undef constructor
#undef destructor
#undef update
#undef Idle
#undef Activated
#undef stateIdle
#undef stateActivated

#define playerEventHandler sonic_playerEventHandler
#define input sonic_input
#define constructor sonic_constructor
#define update sonic_update
#define fsmState sonicFsmState
#include "../../game/sonic.c"
#undef playerEventHandler
#undef input
#undef constructor
#undef update
#undef fsmState 

#include "../../game/actors.c"
#include "../../game/enemyupdate.c"


#include "../../smb3scene.h"
#include "../../smb3col.h"


struct m_image sonic_image = M_IMAGE_IDENTITY();

extern struct m_image buffer;
int fullscreen = 0;

typedef struct {
    float r,g,b;
}CToy_ClearColor;

static CToy_ClearColor ctoyClearColor;

void setClearColor(uint8_t r, uint8_t g, uint8_t b) {
    ctoyClearColor.r = r / 255.f;
    ctoyClearColor.g = g / 255.f;
    ctoyClearColor.b = b / 255.f;
}

void clear(struct m_image *dest) {

    float r = ctoyClearColor.r;//0.5;//175 / 255.0f;
    float g = ctoyClearColor.g;//0.9;//249 / 255.0f;
    float b = ctoyClearColor.b;//0.9;//240 / 255.0f;

    //float r = 0.5;// This gives r 188 colors are not exact!
    //float g = 0.5;// This gives g 188
    //float b = 0.5;// This gives b 188

    float *pixel = (float *)dest->data;
    int numPixels = dest->height * dest->width;
    for (int i = 0; i < numPixels; i++) {
        pixel[0] = r;
        pixel[1] = g;
        pixel[2] = b;

        pixel += dest->comp;
    }
}

void blit(struct m_image *dest, const struct m_image *src, int px, int py)
{
    //m_image_copy(&buffer, &sonic_image); //This thing scales to fit the destination image
    //m_image_copy_sub_image(&buffer, &sonic_image, 0, 0, 48, 48); //This thing scales to fit the destination image
    //m_image_resize(&buffer, &sonic_image, 48,48);
    //float *pixel = (float *)buffer.data;

    int offsetX = -px;
    int offsetY = -py;

    //destination rectangle
    int destX = 0/* + 32*/;
    int maxDestX = dest->width/* - 32*/;
    int destY = 0/* + 32*/;
    int maxDestY = dest->height/* - 32*/;

    int numCols = src->width;
    int numRows = src->height;

    int minX = offsetX;
    int maxX = (((maxDestX - destX) + offsetX)) + 1;

    int initialX = CLAMP(minX, 0, numCols);
    int finalX = CLAMP(maxX, 0, numCols);

    int minY = offsetY;
    int maxY = (((maxDestY - destY) + offsetY)) + 1;

    int initialY = CLAMP(minY, 0, numRows);
    int finalY = CLAMP(maxY, 0, numRows);

    int posXBase = destX - offsetX;
    int posYBase = destY - offsetY;

    for(int y = initialY; y < finalY; y++) {
        float *dest_pixel = ((float *)dest->data) + ((posYBase + y) * dest->width + posXBase + initialX) * dest->comp;
        float *src_pixel = ((float *)src->data) + (y * src->width + initialX) * src->comp;
        for(int x = initialX; x < finalX; x++) {
            if (src_pixel[3] > 0.0 ) {
                dest_pixel[0] = src_pixel[0];
                dest_pixel[1] = src_pixel[1];
                dest_pixel[2] = src_pixel[2];
            }
            dest_pixel += dest->comp;
            src_pixel += src->comp;
        }
    }
}

void blit_ex(struct m_image *dest, const struct m_image *src, int px, int py, int sx, int sy, int w, int h, int flags)
{
    int offsetX = -px;
    int offsetY = -py;

    //destination rectangle
    int destX = 0 /*+ 32*/;
    int maxDestX = dest->width/* - 32*/;
    int destY = 0 /*+ 32*/;
    int maxDestY = dest->height/* - 32*/;

    int numCols = w;//src->width;
    int numRows = h;//src->height;

    int minX = offsetX;
    int maxX = (((maxDestX - destX) + offsetX)) /*+ 1*/;

    int initialX = CLAMP(minX, 0, numCols);
    int finalX = CLAMP(maxX, 0, numCols);

    int minY = offsetY;
    int maxY = (((maxDestY - destY) + offsetY)) /**+ 1*/;

    int initialY = CLAMP(minY, 0, numRows);
    int finalY = CLAMP(maxY, 0, numRows);

    int posXBase = destX - offsetX;
    int posYBase = destY - offsetY;

    int mirroredHorizontal = flags & 1;
    int mirroredVertical = flags & 2;

    if (flags == 0) {  
        for(int y = initialY; y < finalY; y++) {
            float *dest_pixel = ((float *)dest->data) + ((posYBase + y) * dest->width + posXBase + initialX) * dest->comp;
            float *src_pixel = ((float *)src->data) + ((y + sy) * src->width + initialX + sx ) * src->comp;
            for(int x = initialX; x < finalX; x++) {
                if (src_pixel[3] > 0.0 ) {
                    dest_pixel[0] = src_pixel[0];
                    dest_pixel[1] = src_pixel[1];
                    dest_pixel[2] = src_pixel[2];
                }
                dest_pixel += dest->comp;
                src_pixel += src->comp;
            }
        }
    } else if (mirroredHorizontal && mirroredVertical) { 
        for(int y = initialY; y < finalY; y++) {
            float *dest_pixel = ((float *)dest->data) + ((posYBase + y) * dest->width + posXBase + initialX) * dest->comp;
            float *src_pixel = ((float *)src->data) + ((h - 1 - y + sy) * src->width + (w - 1 + sx) ) * src->comp;
            src_pixel -= initialX * src->comp;
            for(int x = initialX; x < finalX; x++) {
                if (src_pixel[3] > 0.0 ) {
                    dest_pixel[0] = src_pixel[0];
                    dest_pixel[1] = src_pixel[1];
                    dest_pixel[2] = src_pixel[2];
                }
                dest_pixel += dest->comp;
                src_pixel -= src->comp;
            }
        }
    } else if (mirroredVertical) {
        for(int y = initialY; y < finalY; y++) {
            float *dest_pixel = ((float *)dest->data) + ((posYBase + y) * dest->width + posXBase + initialX) * dest->comp;
            float *src_pixel = ((float *)src->data) + ((h - 1 - y + sy) * src->width + initialX + sx ) * src->comp;
            for(int x = initialX; x < finalX; x++) {
                if (src_pixel[3] > 0.0 ) {
                    dest_pixel[0] = src_pixel[0];
                    dest_pixel[1] = src_pixel[1];
                    dest_pixel[2] = src_pixel[2];
                }
                dest_pixel += dest->comp;
                src_pixel += src->comp;
            }
        }
        //TODO: al salir el sprite por la derecha se corta mal (ver abeja)
    } else if (mirroredHorizontal) {
        for(int y = initialY; y < finalY; y++) {
            float *dest_pixel = ((float *)dest->data) + ((posYBase + y) * dest->width + posXBase + initialX) * dest->comp;
            float *src_pixel = ((float *)src->data) + ((y + sy) * src->width + (w - 1 + sx) ) * src->comp;
            src_pixel -= initialX * src->comp;
            for(int x = initialX; x < finalX; x++) {
                if (src_pixel[3] > 0.0 ) {
                    dest_pixel[0] = src_pixel[0];
                    dest_pixel[1] = src_pixel[1];
                    dest_pixel[2] = src_pixel[2];
                }
                dest_pixel += dest->comp;
                src_pixel -= src->comp;
            }
        }
    }
}


void ctoy_fadeToBlack(struct m_image *dest, float fade) {
    for(int y = 0; y < dest->height; y++) {
        float *dest_pixel = ((float *)dest->data) + y * dest->width * dest->comp;
        for(int x = 0; x < dest->width; x++) {
            dest_pixel[0] = dest_pixel[0] * fade;
            dest_pixel[1] = dest_pixel[1] * fade;
            dest_pixel[2] = dest_pixel[2] * fade;
            dest_pixel += dest->comp;
        }
    }
}

void ctoy_fadeToWhite(struct m_image *dest, float fade) {
    for(int y = 0; y < dest->height; y++) {
        float *dest_pixel = ((float *)dest->data) + y * dest->width * dest->comp;
        for(int x = 0; x < dest->width; x++) {
            dest_pixel[0] = dest_pixel[0] + (1.0 - dest_pixel[0]) * fade;
            dest_pixel[1] = dest_pixel[1] + (1.0 - dest_pixel[1]) * fade;
            dest_pixel[2] = dest_pixel[2] + (1.0 - dest_pixel[2]) * fade;
            dest_pixel += dest->comp;
        }
    }
}

void fadeToBlack(uint8_t fade) {
    ctoy_fadeToBlack(&buffer, 1.0 - fade/255.0f);
}

void fadeToWhite(uint8_t fade) {
    ctoy_fadeToWhite(&buffer, fade/255.0f);
}

void *persistent_memory = NULL;

typedef struct {
    int x,y;
}SaveGameState;

static TileMap bgaTileMap;
static TileMap collisionTileMap;

static int tileset_fpgs[4];

static AnimationState* tilesetAnimationState;

//#define TILE_SIZE 16
//#define HALF_TILE_SIZE (TILE_SIZE/2)
//#define TILE(X) FIX32(X*TILE_SIZE)

void ctoy_begin(void)
{
    if (ctoy_t() == 0) {
        persistent_memory = calloc(sizeof (SaveGameState), 1); // allocate 256 bytes with zero value
        ctoy_register_memory(persistent_memory); // register persistent memory

        SaveGameState * gs = persistent_memory;
        gs->y = 188;
    }
    else {
        persistent_memory = ctoy_retrieve_memory(); // retrieve persistent memory
    }

    printf("--- ctoy_begin ---\n");
    gameInit();
    initResources();

    gameTitle();
    //gameMain();
    return;

    HGL_init();

    HGL_COMMAND_init();
    HGL_SCRIPT_init();
    HGL_ANIM_init();
    HGL_SPR_init();
    HGL_ENT_init();
    HGL_ACTOR_init();

    //init_fpgs();

    tileset_fpgs[0] = new_fpg();
    tileset_fpgs[1] = new_fpg();
    tileset_fpgs[2] = new_fpg();
    tileset_fpgs[3] = new_fpg();

    int tiles_fpg = tileset_fpgs[0];

    int bga_map = load_atlas(tileset_fpgs[0], "art/smb3t" , 16, 16, 11, 9);
    load_atlas(tileset_fpgs[1], "art/smb3t2", 16, 16, 11, 9);
    load_atlas(tileset_fpgs[2], "art/smb3t3", 16, 16, 11, 9);
    load_atlas(tileset_fpgs[3], "art/smb3t4", 16, 16, 11, 9);

    ANIM(TilesetAnimation, 0, 1, 2, 3)

    tilesetAnimationState = HGL_ANIM_new();
    SetAnimationState(tilesetAnimationState, TilesetAnimation, 8);

    int enemies_fpg = new_fpg();
    int enemies_map = load_atlas(enemies_fpg, "art/enemies", 48, 32, 4, 2);
    newMotobug(enemies_fpg, TILE(20),TILE(25));
    newMotobug(enemies_fpg, TILE(64),TILE(24));
    newMotobug (enemies_fpg, TILE(90), TILE(25));
    newBee(enemies_fpg, TILE(130),TILE(19));

    m_image_load_float(&sonic_image, "art/sonic.png");

    bgaTileMap = fromTiledBin(smb3_2_layer);
    collisionTileMap = fromTiledBin(smb3_2_collision);
}

void ctoy_end(void)
{
    printf("--- ctoy_end ---\n");
   //m_image_destroy(&sonic_image);
   //m_image_destroy(&buffer);
   //TODO: FREE ALL RESOURCES
}

int anim[] = { 1,2,3,4,0, 1 };
int anim2[] = { 0 , 0, 0, 0 , 1, 1 };

int matri = 0;
int frames = 0;

void ctoy_main_loop(void)
{
   // enter / exit fullscreen
   if (ctoy_key_press(CTOY_KEY_F)) {
      fullscreen = !fullscreen;
      ctoy_window_fullscreen(fullscreen);
      if (!fullscreen) ctoy_window_size(512, 512);
   }

   // exit
   if (ctoy_key_press(CTOY_KEY_Q)) {
      exit(EXIT_SUCCESS);
      return;
   }
   // printf("ctoy_main_loop\n");
   gameTitleUpdate();
   //gameUpdate();
   return;
/*
    SaveGameState *state = (SaveGameState *)persistent_memory; // access a piece of persistent memory

    //gameUpdate();
    //return;

    uint16_t button = getButtons(0);

    if (button & PAD_LEFT) {
        state->x--;
    } else if (button & PAD_RIGHT) {
        state->x++;
    }
    if (button & PAD_UP) {
        state->y--;
    } else if (button & PAD_DOWN) {
        state->y++;
    }

    frames++;
    if(frames>4) {
        matri++;
        if(matri>5) matri = 0;
        frames = 0;
    }

    int sx = anim[matri] * 48;
    int sy = anim2[matri] * 48;

    int bgbx = state->x;
    int bgby = state->y;

    HGL_COMMAND_updateAll();
    HGL_SCRIPT_updateAll();
    HGL_ANIM_updateAll();
    HGL_ENT_updateAll();
    HGL_ACTOR_updateAll();
    HGL_ENT_renderAll(bgbx,bgby);
    HGL_SPR_renderAll();

    //draw_all_sprites_basic();

    //update_Particles();

    clear(&buffer);
    //blit(&buffer, &smb3t, 0, 0);
    //draw_tilemap_no_wrap(tileset_fpgs[tilesetAnimationState->currentFrame], 1, &bgaTileMap, bgbx, bgby, 0); //Front
    draw_tilemap_no_wrap(tileset_fpgs[tilesetAnimationState->currentFrame], 1, &bgaTileMap, bgbx, bgby, 0); //Front
    //draw_tilemap_no_wrap(tileset_fpgs[0], 1, &bgaTileMap, bgbx, bgby, 0); //Front

    draw_all_sprites_basic(); //Z order is not working yet, so drawing sprites later

    blit_ex(&buffer, &sonic_image, state->x, state->y, sx, sy, 48, 48, 0);

    HGL_frame();*/

}
