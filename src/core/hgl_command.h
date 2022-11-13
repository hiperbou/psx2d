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


//commands
void SetUint8(DelayedCommand*) ;


#endif //DEMO_HGL_COMMAND_H
