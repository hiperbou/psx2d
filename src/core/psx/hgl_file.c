#include <stdint.h>
#include <string.h>
#include "../hgl_file.h"
#include "../../utils/utils.h"
#include "../../psx/system.h"

void GetSpriteFromDisc(char *filename, SPRITE *sprite) {
    TIM_IMAGE timImage;

    char discFilename[64];
    strcpy(discFilename, "\\");
    replace_char(filename, '/', '\\');
    to_uppercase(filename);
    strcat(discFilename, filename);
    strcat(discFilename, ".TIM;1");

    LoadTextureFromDisc(discFilename, &timImage);
    GetSprite(&timImage, sprite);
}

void GetSpriteFromMemory(uint8_t *data, SPRITE *sprite) {
    TIM_IMAGE timImage;
    LoadTexture((u_long *)data, &timImage);
    GetSprite(&timImage, sprite);
}