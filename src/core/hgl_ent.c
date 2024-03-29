#include "hgl_ent.h"
#include "../pool/ObjectPool.h"

//TODO: replace this defines with a parameter in the HGL_XXX_init functions
#define MAX_NUM_ENTITIES 40

static ObjectPool * entityPool;

static u16 scrollOffsetX = 0;
static u16 scrollOffsetY = 0;

typedef struct EntityItem {
    HGL_Entity entity;
    int itemID;
}EntityItem;

static void entityInitializer(HGL_Entity * entity, int i) {
    *entity = (HGL_Entity) {
            .x = 0,
            .y = 0,
            .state = ENT_STATE_DEAD,
            .spr = NULL
    };
}

//TODO: every HGL_XXX_init function needs a HGL_XXX_dispose to clean the pool
void HGL_ENT_init()
{
    entityPool = new_ObjectPool(MAX_NUM_ENTITIES, sizeof(HGL_Entity));

    ObjectPool_initialize_indexed(entityPool, (PoolInitializerCallback*) entityInitializer);
}

HGL_Entity* HGL_ENT_new(const fix32 x, const fix32 y, HGL_Sprite* spr, EntityUpdateCallback* updateCB)
{
	PoolElement * elem = ObjectPool_get(entityPool);
	HGL_Entity * ent = elem->data;
    ent->x = x;
	ent->y = y;
	ent->state = ENT_STATE_ACTIVE;
	ent->updateCallback = (void*)updateCB;
	ent->spr = spr;

    return ent;
}

void HGL_ENT_delete(HGL_Entity *e)
{
	e->state = ENT_STATE_DEAD;
	if (e->spr) HGL_SPR_delete(e->spr);

	ObjectPool_free(entityPool, e);
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

void HGL_ENT_updateAll()
{
    OBJECTPOOL_ITERATOR_IF_START(entityPool, HGL_Entity, it->state != ENT_STATE_DEAD)
        HGL_ENT_update(it);
    OBJECTPOOL_ITERATOR_IF_END
}

void HGL_ENT_renderAll(u16 offsetX, u16 offsetY)
{
    scrollOffsetX = offsetX;
    scrollOffsetY = offsetY;

    OBJECTPOOL_ITERATOR_IF_START(entityPool, HGL_Entity, it->state != ENT_STATE_DEAD)
        HGL_ENT_render(it);
    OBJECTPOOL_ITERATOR_IF_END
}

void HGL_ENT_setPosition(HGL_Entity *e, fix32 x, fix32 y) {
    e->x = x;
    e->y = y;
}
