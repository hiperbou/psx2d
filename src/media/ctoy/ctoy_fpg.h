#ifndef CTOY_1_06_CTOY_FPG_H
#define CTOY_1_06_CTOY_FPG_H
#include <ctoy.h>

typedef struct m_image CToyImage;

typedef struct SPRITE {
    CToyImage m_image;
    int32_t u,v;
    int32_t w,h;
} SPRITE;

#endif //CTOY_1_06_CTOY_FPG_H
