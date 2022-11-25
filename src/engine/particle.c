#include <stddef.h>
#include "particle.h"
//#include "pool.h"
#include "sprites.h"
#include "rand.h"
#include "hgl_pool.h"
#include "ObjectPool.h"

#define MAX_NUM_PARTICLES 128

static ObjectPool * particlePool;

typedef struct ParticleItem {
    Particle particle;
    int itemID;
}ParticleItem;

static void initPool() {
    particlePool = new_ObjectPool(MAX_NUM_PARTICLES, sizeof(Particle));
    /*ObjectPool_initialize(particlePool, &(Particle) {
            .active = 0
    }, sizeof(Particle));*/
}

void init_particles() {
    printf("init Particles\n");
    //srand(123);
    initPool();
}

int random(int min, int max) {
    return rand() %((max + 1 ) - min) + min;
}

void init_particle(Particle *particle, int x, int y) {
    *particle = (Particle) {
            //.id = particle->id,

            //.active = 1,
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
    //printf("particle.v.x %i\n", particle->speedY.x);
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
    return p;
}

void remove_Particle(Particle *particle) {
    ObjectPool_free(particlePool, particle);
}

static inline void particleUpdater(Particle * particle) {
    update_particle(particle);
    draw_particle(particle);
    if (particle->life <= 0 ||
        particle->pos.x < 0 || particle->pos.x > FIX32(320) ||
        particle->pos.y < 0 || particle->pos.y > FIX32(240)) {
        //particle->active = 0;
        remove_Particle(particle);
    }
}


void update_Particles() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(particlePool, Particle)
    particleUpdater(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
    //printf("iterations %i\n", _i);
}

void remove_Particles() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(particlePool, Particle)
            remove_Particle(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
    //printf("iterations %i\n", _i);
}
