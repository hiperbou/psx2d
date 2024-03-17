#include <stdint.h>
#include "../hgl_file.h"
#include "../../media/fpg.h"
#include "../../utils/utils.h"
#include <raylib.h>
#include <string.h>

void GetSprite(Texture *tim, SPRITE *sprite) {
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
    char discFilename[64];
    strcpy(discFilename, "");
    //replace_char(filename, '/', '\\');
    //to_uppercase(filename);
    strcat(discFilename, filename);
    strcat(discFilename, ".png");

    Texture2D image = LoadTexture((const char*)discFilename); 
    GetSprite(&image, sprite);
}

void GetSpriteFromMemory(uint8_t *data, SPRITE *sprite) {
    //TIM_IMAGE timImage;
    //LoadTexture((u_long *)data, &timImage);
    //GetSprite(&timImage, sprite);
}
