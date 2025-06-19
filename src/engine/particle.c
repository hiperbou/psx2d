#include <stddef.h>
#include "particle.h"
//#include "pool.h"
#include "sprites.h"
#include <stdlib.h> //rand
//#include "rand.h"
#include "../core/hgl_pool.h"
#include "../game/camera.h"
#include "../pool/ObjectPool.h"

#define MAX_NUM_PARTICLES 128

static ObjectPool * particlePool;

typedef struct ParticleItem {
    Particle particle;
    int itemID;
}ParticleItem;

static void initPool() {
    particlePool = new_ObjectPool(MAX_NUM_PARTICLES, sizeof(Particle));
}

void init_particles() {
    //printf("init Particles\n");
    //srand(123);
    initPool();
}

inline static int particle_random(int min, int max) {
    return rand() %((max + 1 ) - min) + min;
}

void init_particle(Particle *particle, int x, int y) {
    *particle = (Particle) {
            .pos.x = FIX32(x),
            .pos.y = FIX32(y),
            .speed.x = particle_random(FIX32(-2), FIX32(2)),
            .speed.y = particle_random(FIX32(-5), 0),
            .gravity.x = 0,
            .gravity.y = FIX32(0.1),
            .size = 100,
            .incrSize = -particle_random(0, 2),
            .angle = particle_random(0, FIX32(1)),
            .incrAngle = particle_random(-109, 109),

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

	#define SCREEN_WIDTH 320
	#define SCREEN_HEIGHT 240
	int screen_x = (particle->pos.x >> 12) - camposx;
	int screen_y = (particle->pos.y >> 12) - camposy;

    if ((particle->pos.x >> 12) < camposx ||
        (particle->pos.x >> 12) > camposx + SCREEN_WIDTH ||
        (particle->pos.y >> 12) < camposy ||
        (particle->pos.y >> 12) > camposy + SCREEN_HEIGHT ||
        particle->life <= 0 ||
        particle->size <= 0) {
        remove_Particle(particle);
    } else {
		draw_particle(2, 42, screen_x, screen_y, particle);
	}
}


void update_Particles() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(particlePool, Particle)
        particleUpdater(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}

void remove_Particles() {
    OBJECTPOOL_ITERATOR_ALLOCATED_START(particlePool, Particle)
        remove_Particle(it);
    OBJECTPOOL_ITERATOR_ALLOCATED_END
}
