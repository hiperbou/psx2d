
#ifndef RAYLIB_FPG_H
#define RAYLIB_FPG_H
#include "raylib.h"


typedef struct SPRITE {
    Texture2D m_image;
    int  u,v;    // UV offset (useful for non page aligned TIMs)
    int   w,h;    // Size (primitives can only draw 256x256 anyway)
    //CVECTOR col;
} SPRITE;

#endif //RAYLIB_FPG_H