#ifndef DEMO_HGL_COMMAND_H
#define DEMO_HGL_COMMAND_H

#include <stdint.h>

typedef struct DelayedCommand DelayedCommand;

typedef void DelayedCommandCallback(DelayedCommand* data);

typedef struct DelayedCommand{
    int delay;
    DelayedCommandCallback *callback;
    void * target;
    int data;
}DelayedCommand;

void HGL_COMMAND_init();
DelayedCommand* HGL_COMMAND_new();
void HGL_COMMAND_delete(DelayedCommand *e);
void HGL_COMMAND_updateAll();
void HGL_COMMAND_deleteAll();


//commands
void SetUint8(DelayedCommand*) ;
void newSetUint8DelayedCommand(int delay, uint8_t *target, uint8_t value);


#endif //DEMO_HGL_COMMAND_H
