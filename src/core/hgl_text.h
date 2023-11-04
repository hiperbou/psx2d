#ifndef HGL_TEXT_H_
#define HGL_TEXT_H_

#include "hgl_pool.h"
#include "hgl_types.h"

typedef struct{
    int file;
    int base_map;
    int x;
    int y;
    const char * text;
    int flags;
    int length;
}HGL_Text;

void HGL_TEXT_init();
//void HGL_TEXT_update();

void HGL_TEXT_renderAll();

HGL_Text* HGL_TEXT_new(int file, int base_map, const char* text, int x, int y, int flags, int length);

void HGL_TEXT_delete(HGL_Text * sprite);
void HGL_TEXT_deleteAll();


#endif /* HGL_TEXT_H_ */
