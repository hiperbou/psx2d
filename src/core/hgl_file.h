#ifndef DEMO_HGL_FILE_H
#define DEMO_HGL_FILE_H

#include <stdint.h>
#include "../media/fpg.h"

void GetSpriteFromDisc(char *filename, SPRITE *sprite);
void GetSpriteFromMemory(uint8_t *data, SPRITE *sprite);

#endif //DEMO_HGL_FILE_H
