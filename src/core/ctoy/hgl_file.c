#include <stdint.h>
#include "../hgl_file.h"
#include "../../media/fpg.h"
#include "../../utils/utils.h"
#include <ctoy.h>

void GetSprite(struct m_image *tim, SPRITE *sprite) {
    sprite->m_image = *tim;
    // Set sprite size
    sprite->w = tim->width;
    sprite->h = tim->height;

    // Set UV offset
    sprite->u = 0;
    sprite->v = 0;

    // Set neutral color
    //sprite->col.r = 128;
    //sprite->col.g = 128;
    //sprite->col.b = 128;
}

void GetSpriteFromDisc(char *filename, SPRITE *sprite) {
    //TIM_IMAGE timImage;
    struct m_image image = M_IMAGE_IDENTITY();

    char discFilename[64];
    strcpy(discFilename, "");
    //replace_char(filename, '/', '\\');
    //to_uppercase(filename);
    strcat(discFilename, filename);
    strcat(discFilename, ".png");

    //LoadTextureFromDisc(discFilename, &timImage);
    m_image_load_float(&image, (const char*)discFilename);
    GetSprite(&image, sprite);
}

void GetSpriteFromMemory(uint8_t *data, SPRITE *sprite) {
    //TIM_IMAGE timImage;
    //LoadTexture((u_long *)data, &timImage);
    //GetSprite(&timImage, sprite);
}
