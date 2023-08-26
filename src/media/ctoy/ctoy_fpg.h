
#ifndef CTOY_1_06_CTOY_FPG_H
#define CTOY_1_06_CTOY_FPG_H
#include <ctoy.h>

typedef struct m_image CToyImage;

typedef struct SPRITE {
    CToyImage m_image;
    int32_t  u,v;    // UV offset (useful for non page aligned TIMs)
    int32_t   w,h;    // Size (primitives can only draw 256x256 anyway)
    //CVECTOR col;
} SPRITE;

#endif //CTOY_1_06_CTOY_FPG_H
