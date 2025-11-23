#ifndef OPENGL2_BACKEND_H
#define OPENGL2_BACKEND_H

typedef unsigned int GLuint;

typedef struct SPRITE {
    GLuint texture_id;
    int u, v;
    int w, h;
    int tex_w, tex_h;
    float minU, minV, maxU, maxV;
} SPRITE;

#endif // OPENGL2_BACKEND_H
