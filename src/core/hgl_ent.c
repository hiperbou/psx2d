#include "hgl_ent.h"
#include "ObjectPool.h"
/*
#define MAX_SPRITE 40 //MAX_VDP_SPRITE
#define MAX_NUM_ENTITIES MAX_SPRITE

static u16  activeEntities = 0;

static ObjectPool * entityPool;

u16 scrollOffsetX = 0;
u16 scrollOffsetY = 0;

typedef struct EntityItem {
    HGL_Entity entity;
    int itemID;
}EntityItem;

static void entityInitializer(HGL_Entity * entity, int i) {
    *entity = (HGL_Entity) {
            .x = 0,
            .y = 0,
            .state = ENT_STATE_DEAD,
            .id = i,
            .user0 = 0,
            .spr = NULL
    };
}

void HGL_ENT_init()
{
    activeEntities = 0;
    entityPool = new_ObjectPool(MAX_NUM_ENTITIES, sizeof(HGL_Entity));

    ObjectPool_initialize_indexed(entityPool, (PoolInitializerCallback*) entityInitializer);
}

HGL_Entity* HGL_ENT_new2(const fix32 x, const fix32 y, HGL_Sprite* spr, EntityUpdateCallback* updateCB, Actor * actor)
{
	PoolElement * elem = ObjectPool_get(entityPool);
	HGL_Entity * ent = elem->data;
	ent->id = elem->id;
    ent->x = x;
	ent->y = y;
	ent->state = ENT_STATE_ACTIVE;
	ent->updateCallback = (void*)updateCB;
	ent->user0 = 0;
	ent->actor = actor;
	ent->spr = spr;

	activeEntities++; //entityPool->size;
    return ent;
}

void HGL_ENT_delete(HGL_Entity *e)
{
	e->state = ENT_STATE_DEAD;
	if (e->spr) HGL_SPR_delete(e->spr);
	ObjectPool_free(entityPool, e);
    activeEntities--;
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




void HGL_ENT_updateAll(u16 offsetX,u16 offsetY) {
    OBJECTPOOL_ITERATOR_IF_START(entityPool, HGL_Entity, it->state != ENT_STATE_DEAD)
       HGL_ENT_update(it);
    OBJECTPOOL_ITERATOR_IF_END
    /*int i = 0;
    int entitiesToProcess = activeEntities;
    EntityItem * entityItem = entityPool->objects;
    while (i < MAX_NUM_ENTITIES && entitiesToProcess > 0){
        HGL_Entity * entity = (HGL_Entity *)entityItem;
        if (entity->state != ENT_STATE_DEAD)
        {
            entitiesToProcess--;
            HGL_ENT_update(entity);
        }
        entityItem++;
        i++;
    }*/
    //printf("hgl processed %i iterations %i\n", active_particles, i);
/*}*/

/*void HGL_ENT_updateAll(u16 offsetX,u16 offsetY)
{
    scrollOffsetX = offsetX;
    scrollOffsetY = offsetY;
    u16 i=0;

    for (i=0; i<MAX_NUM_ENTITIES && i < activeEntities; i++){
        if(hglEntities[i].state != ENT_STATE_DEAD)
        {
            HGL_ENT_update(&hglEntities[i]);
        }
    }
}*//*

void HGL_ENT_renderAll(u16 offsetX,u16 offsetY)
{
    OBJECTPOOL_ITERATOR_IF_START(entityPool, HGL_Entity, it->state != ENT_STATE_DEAD)
        HGL_ENT_render(it);
    OBJECTPOOL_ITERATOR_IF_END*/

    /*scrollOffsetX = offsetX;
    scrollOffsetY = offsetY;
    u16 i=0;

    for (i=0; i<MAX_NUM_ENTITIES && i < activeEntities; i++){
        if(hglEntities[i].state != ENT_STATE_DEAD)
        {
            HGL_ENT_render(&hglEntities[i]);
        }
    }*//*
}

void HGL_ENT_setPosition(HGL_Entity *e, fix32 x, fix32 y) {
    e->x = x;
    e->y = y;
}
*/

