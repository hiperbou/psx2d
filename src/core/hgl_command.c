#include "hgl_command.h"
#include "../pool/ObjectPool.h"

#include <stdio.h>	

#define MAX_NUM_COMMANDS 64

static ObjectPool * commandPool;

void SetUint8(DelayedCommand * data) {
    uint8_t* dest = data->target;
    uint8_t value = data->data;
    *dest = value;
}

void newSetUint8DelayedCommand(int delay, uint8_t *target, uint8_t value) {
    DelayedCommand * command = HGL_COMMAND_new();
    *command = (DelayedCommand) {
            .delay = delay,
            .callback = SetUint8,
            .target = target,
            .data = value
    };
}

void HGL_COMMAND_init()
{
    commandPool = new_ObjectPool(MAX_NUM_COMMANDS, sizeof(DelayedCommand));
}

inline static void HGL_COMMAND_reset(DelayedCommand *delayedCommand) {
    *delayedCommand = (DelayedCommand) {
        .delay = 0,
        .callback = NULL,
        .target = NULL,
        .data = 0
    };
}

DelayedCommand* HGL_COMMAND_new()
{
    PoolElement * elem = ObjectPool_get(commandPool);
    HGL_COMMAND_reset(elem->data);
    return elem->data;
}

DelayedCommand* HGL_COMMAND_create(int delay,  DelayedCommandCallback *callback,  void * target,  int data)
{
    DelayedCommand * command = HGL_COMMAND_new();
    *command = (DelayedCommand) {
        .delay = delay,
        .callback = callback,
        .target = target,
        .data = data
    };
    return command;
}

void HGL_COMMAND_delete(DelayedCommand *delayedCommand)
{
    ObjectPool_free(commandPool, delayedCommand);
}

static inline void updateCommandState(DelayedCommand * delayedCommand) {
    delayedCommand->delay--;
    if (delayedCommand->delay <= 0) {
        //printf("command executed!\n");
        if(delayedCommand->callback) {
            delayedCommand->callback(delayedCommand);
        }
        HGL_COMMAND_delete(delayedCommand);
    }
}

void HGL_COMMAND_updateAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(commandPool, DelayedCommand)
        updateCommandState(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

void HGL_COMMAND_deleteAll() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(commandPool, DelayedCommand)
        HGL_COMMAND_delete(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

