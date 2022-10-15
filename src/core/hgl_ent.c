#include "hgl_ent.h"
/*
#define MAX_SPRITE 40 //MAX_VDP_SPRITE
#define MAX_NUM_ENTITIES MAX_SPRITE

static HGL_Entity hglEntities[MAX_NUM_ENTITIES];
static PoolElement entityElement[MAX_NUM_ENTITIES];
static Pool entityPool;
static u16  totalEntities = 0;

u16 scrollOffsetX = 0;
u16 scrollOffsetY = 0;

void HGL_ENT_init()
{
	totalEntities = 0;
	HGL_POOL_createPool(&entityPool, MAX_SPRITE, hglEntities, sizeof(HGL_Entity), entityElement);

    u16 i=0;
    for (i=0; i<MAX_NUM_ENTITIES; i++){
    	hglEntities[i].x = 0;
    	hglEntities[i].y = 0;
    	hglEntities[i].state = ENT_STATE_DEAD;
    	hglEntities[i].id = i;
    	hglEntities[i].user0 = 0;
    	hglEntities[i].spr = NULL;
    }
}

HGL_Entity* HGL_ENT_new2(const fix32 x, const fix32 y, HGL_Sprite* spr, EntityUpdateCallback* updateCB, Actor * actor)
{
	PoolElement * elem = HGL_POOL_get(&entityPool);
	HGL_Entity * ent = elem->data;
	ent->id = elem->id;
    ent->x = x; //+ 0x80;
	ent->y = y; //+ 0x80;
	ent->state = ENT_STATE_ACTIVE;
	ent->updateCallback = (void*)updateCB;
	ent->user0 = 0;
	ent->actor = actor;
	ent->spr = spr;

	totalEntities = entityPool.size;
    return ent;
}

void HGL_ENT_delete(HGL_Entity *e)
{
	e->state = ENT_STATE_DEAD;
	if(e->spr) HGL_SPR_delete(e->spr);
	HGL_POOL_free(&entityPool, &entityElement[e->id]);
}

void HGL_ENT_update(HGL_Entity *e)
{
    if(e->updateCallback) e->updateCallback(e);
}

void HGL_ENT_render(HGL_Entity *e)
{
    if(e->spr==NULL) return;
    HGL_SPR_setPosition(e->spr, fix32ToInt(e->x) - scrollOffsetX, fix32ToInt(e->y) - scrollOffsetY);
}

void HGL_ENT_updateAll(u16 offsetX,u16 offsetY)
{
    scrollOffsetX = offsetX;
    scrollOffsetY = offsetY;
    u16 i=0;

    for (i=0; i<MAX_NUM_ENTITIES && i<totalEntities; i++){
        if(hglEntities[i].state != ENT_STATE_DEAD)
        {
            HGL_ENT_update(&hglEntities[i]);
        }
    }
}

void HGL_ENT_renderAll(u16 offsetX,u16 offsetY)
{
    scrollOffsetX = offsetX;
    scrollOffsetY = offsetY;
    u16 i=0;

    for (i=0; i<MAX_NUM_ENTITIES && i<totalEntities; i++){
        if(hglEntities[i].state != ENT_STATE_DEAD)
        {
            HGL_ENT_render(&hglEntities[i]);
        }
    }
}

void HGL_ENT_setPosition(HGL_Entity *e, fix32 x, fix32 y) {
    e->x = x;
    e->y = y;
}


*/