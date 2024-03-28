#ifndef DEMO_HGL_SCRIPT_H
#define DEMO_HGL_SCRIPT_H

#include <stdint.h>
#include "../utils/script.h"

/*
async asyncMainLoop(struct async *mainLoopAsyncState) {
    async_begin(mainLoopAsyncState);
    await(gameUpdate());
    async_end;
}*/

typedef struct async AsyncState;
typedef struct AsyncScript AsyncScript;

typedef async AsyncScriptCallback(AsyncScript* asyncScript);

typedef struct AsyncScript{
    int delay;
    AsyncScriptCallback *callback;
    struct async asyncState;
    void * target;
    int data;
}AsyncScript;

void HGL_SCRIPT_init();
AsyncScript* HGL_SCRIPT_new();
AsyncScript* HGL_SCRIPT_create(int delay,  AsyncScriptCallback *callback,  void * target,  int data);
void HGL_SCRIPT_delete(AsyncScript *e);
void HGL_SCRIPT_updateAll();
void HGL_SCRIPT_deleteAll();


#endif //DEMO_HGL_SCRIPT_H
