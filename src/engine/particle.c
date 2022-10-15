#include <stddef.h>
#include "particle.h"
//#include "pool.h"
#include "sprites.h"
#include "rand.h"
#include "hgl_pool.h"
#include "ObjectPool.h"

#define MAX_NUM_PARTICLES 128
#define FIX32(X) ((int)((X) * 4096))

static int active_particles = 0;

static ObjectPool * particlePool;

typedef struct ParticleItem {
    Particle particle;
    int itemID;
}ParticleItem;

static void initPool() {
    particlePool = new_ObjectPool(MAX_NUM_PARTICLES, sizeof(ParticleItem));
    ParticleItem * particleItem = particlePool->objects;

    for (int i=0; i<MAX_NUM_PARTICLES; i++){
        Particle * particle = (Particle *)particleItem;//&particleItem->particle;
        particle->active = 0;
        //particle->id = i;
        particleItem++;
    }
}

void init_particles() {
    printf("init Particles");
    //srand(123);
    initPool();
}

int random(int min, int max) {
    return rand() %((max + 1 ) - min) + min;
}

void init_particle(Particle *particle, int x, int y) {
    *particle = (Particle) {
            //.id = particle->id,

            .active = 1,
            .pos.x = FIX32(x),
            .pos.y = FIX32(y),
            .speed.x = random(FIX32(-2), FIX32(2)),
            .speed.y = random(FIX32(-5), 0),
            .gravity.x = 0,
            .gravity.y = FIX32(0.1),
            .size = 100,
            .incrSize = -random(0, 2),
            .angle = random(0, FIX32(1)),
            .incrAngle = random(-109, 109),

            .life = 60 * 3,
            .fade = 1,

            .color.r=255,
            .color.g=255,
            .color.b=255
    };
}

void update_particle(Particle *particle) {
    particle->life -= particle->fade;
    particle->size += particle->incrSize;
    particle->angle += particle->incrAngle;
    //printf("particle %i\n", particle);
    //printf("particle.x %i\n", particle->pos.x);
    //printf("particle.v.x %i\n", particle->speed.x);
    particle->pos.x += particle->speed.x;
    particle->pos.y += particle->speed.y;

    particle->speed.x += particle->gravity.x;
    particle->speed.y += particle->gravity.y;
}

static Particle* newParticleFromPool(){
    PoolElement * elem = ObjectPool_get(particlePool);
    ParticleItem * particleItem = elem->data;
    particleItem->itemID = elem->id;
    return elem->data;
}

Particle * new_Particle(int x, int y) {
    Particle * p = newParticleFromPool();
    init_particle(p, x, y);
    active_particles++;
    return p;
}

void remove_Particle(Particle *particle) {
    active_particles--;
    ObjectPool_free(particlePool, (ObjectPoolItemID *) &((ParticleItem*)particle)->itemID);
}

static void particleUpdater(Particle * particle) {
    update_particle(particle);
    draw_particle(particle);
    if (particle->life <= 0 ||
        particle->pos.x < 0 || particle->pos.x > FIX32(320) ||
        particle->pos.y < 0 || particle->pos.y > FIX32(240)) {
        particle->active = 0;
        remove_Particle(particle);
    }
}

void update_Particles() {
    int i = 0;
    int particlesToProcess = active_particles;
    ParticleItem * particleItem = particlePool->objects;
    while (i < MAX_NUM_PARTICLES && particlesToProcess > 0){
        Particle * particle = (Particle *)particleItem;
        if (particle->active)
        {
            particlesToProcess--;
            particleUpdater(particle);
        }
        particleItem++;
        i++;
    }
    //printf("hgl processed %i iterations %i\n", active_particles, i);
}
