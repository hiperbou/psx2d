#ifndef __HGL_PSX_FPG_H
#define __HGL_PSX_FPG_H

#include <sys/types.h>	// This provides typedefs needed by libgte.h and libgpu.h
#include <libgte.h>	// GTE header, not really used but libgpu.h depends on it

typedef struct _SPRITE {
    u_short tpage;  // Tpage value
    u_short clut;   // CLUT value
    u_char  u,v;    // UV offset (useful for non page aligned TIMs)
    u_char  w,h;    // Size (primitives can only draw 256x256 anyway)
    CVECTOR col;
} SPRITE;

#endif //__HGL_PSX_FPG_H
