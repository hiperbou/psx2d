#include "camera.h"

#include "hgl.h"
#include "hgl_spr.h"
#include "hgl_ent.h"

#include "enemyupdate.h"
#include "actor.h"
#include "hgl_anim.h"


#define MAP_WIDTH           10240
#define MAP_HEIGHT          1280

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
    if (px_scr > 240) npx_cam = px - 240;
    else if (px_scr < 40) npx_cam = px - 40;
    else npx_cam = camposx;
    if (py_scr > 140) npy_cam = py - 140;
    else if (py_scr < 60) npy_cam = py - 60;
    else npy_cam = camposy;

    // clip camera position
    if (npx_cam < 0) npx_cam = 0;
    else if (npx_cam > (MAP_WIDTH - 320)) npx_cam = (MAP_WIDTH - 320);
    if (npy_cam < 0) npy_cam = 0;
    else if (npy_cam > (MAP_HEIGHT - 224)) npy_cam = (MAP_HEIGHT - 224);

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
