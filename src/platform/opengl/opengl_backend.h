#ifndef OPENGL_BACKEND_H
#define OPENGL_BACKEND_H

typedef unsigned int GLuint;

typedef struct SPRITE {
    GLuint texture_id;
    int u, v;
    int w, h;
    int tex_w, tex_h;
} SPRITE;

#endif // OPENGL_BACKEND_H
