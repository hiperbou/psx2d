#ifndef __HGL_PARTICLE_H__
#define __HGL_PARTICLE_H__

typedef struct
{
    int x, y;
}VECTOR2D;

typedef struct
{
    int r,g,b;
}COLOR;

typedef struct Particle{
    int active;
    VECTOR2D pos;
    VECTOR2D speed;
    VECTOR2D gravity;

    int life;
    int fade;

    int size;
    int incrSize;

    int angle;
    int incrAngle;

    COLOR color;
}Particle;

void init_particles();
void update_Particles();

Particle * new_Particle(int x, int y);


#endif //__HGL_PARTICLE_H__
