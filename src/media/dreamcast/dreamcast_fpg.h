#ifndef __HGL_DREAMCAST_FPG_H
#define __HGL_DREAMCAST_FPG_H

#include <kos.h>
#include <dc/pvr.h>

typedef struct _SPRITE {
    pvr_ptr_t texture;
    uint16 w, h;
} SPRITE;

#endif //__HGL_DREAMCAST_FPG_H
