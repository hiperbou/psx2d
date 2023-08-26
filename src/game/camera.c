#include "camera.h"

#include "../core/hgl.h"
#include "../core/hgl_spr.h"
#include "../core/hgl_ent.h"

#include "enemyupdate.h"
#include "../core/hgl_actor.h"
#include "../core/hgl_anim.h"


#define MAP_WIDTH           2944//10240
#define MAP_HEIGHT          432//1280

int camposx;
int camposy;

static Actor* target;

static void updateCamera(int x, int y)
{
    if ((x != camposx) || (y != camposy))
    {
        camposx = x;
        camposy = y;
        //printf("cam x %i\n", x);
    }
}

static void camFollow(fix32 posx, fix32 posy){
    int offsetY = -48;
    // get player position (pixel)
    int px = fix32ToInt(posx);
    int py = fix32ToInt(posy) + offsetY;
    // current sprite position on screen
    int px_scr = px - camposx;
    int py_scr = py - camposy;

    int npx_cam, npy_cam;

    // adjust new camera position
    if (px_scr > 160) npx_cam = px - 160;
    else if (px_scr < 100) npx_cam = px - 100;
    else npx_cam = camposx;
    if (py_scr > 140) npy_cam = py - 140;
    else if (py_scr < 20) npy_cam = py - 20;
    else npy_cam = camposy;

    // clip camera position
    if (npx_cam < 0) npx_cam = 0;
    else if (npx_cam > (MAP_WIDTH - 320)) npx_cam = (MAP_WIDTH - 320);
    if (npy_cam < 0) npy_cam = 0;
    else if (npy_cam > (MAP_HEIGHT - 240)) npy_cam = (MAP_HEIGHT - 240);

    updateCamera(npx_cam, npy_cam);
}

static void update(Actor* ent) {
    camFollow(target->entity->x, target->entity->y);
}

static void constructor(Actor* actor) {
}

Actor* newCamera(Actor* cameraTarget, const fix32 x, const fix32 y) {
    target = cameraTarget;
    camposx = -400;
    camposy = -400;
    camFollow(x, y);
    return newActor(0, 0, x, y, constructor, update);
}
