#include "hgl_script.h"
#include "../pool/ObjectPool.h"

#include <stdio.h>	

#define MAX_NUM_SCRIPTS 64

static ObjectPool * scriptPool;

void HGL_SCRIPT_init() {
    scriptPool = new_ObjectPool(MAX_NUM_SCRIPTS, sizeof(AsyncScript));
}

inline static void HGL_SCRIPT_reset(AsyncScript *asyncScript) {
    *asyncScript = (AsyncScript) {
        .delay = 0,
        .callback = NULL,
        .target = NULL,
        .data = 0,
        .asyncState = (AsyncState){ ASYNC_INIT }
    };
}

AsyncScript* HGL_SCRIPT_new() {
    PoolElement * elem = ObjectPool_get(scriptPool);
    HGL_SCRIPT_reset(elem->data);
    return elem->data;
}

AsyncScript* HGL_SCRIPT_create(int delay,  AsyncScriptCallback *callback,  void * target,  int data) {
    AsyncScript * script = HGL_SCRIPT_new();
    *script = (AsyncScript) {
        .delay = delay,
        .callback = callback,
        .target = target,
        .data = data,
        .asyncState = (AsyncState) { ASYNC_INIT }
    };
    return script;
}

void HGL_SCRIPT_delete(AsyncScript *asyncScript) {
    ObjectPool_free(scriptPool, asyncScript);
}

static inline void updateScriptState(AsyncScript * asyncScript) {
    if (asyncScript->delay > 0) {
        asyncScript->delay--;
    } else if(asyncScript->callback(asyncScript) == ASYNC_DONE) {
        HGL_SCRIPT_delete(asyncScript);
    }
}

void HGL_SCRIPT_updateAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(scriptPool, AsyncScript)
        updateScriptState(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

void HGL_SCRIPT_deleteAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(scriptPool, AsyncScript)
        HGL_SCRIPT_delete(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

