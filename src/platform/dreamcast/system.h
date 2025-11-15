#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <kos.h>

void initDisplay();
void initCD();
void initMemory();
void display();
void initFont();

pvr_ptr_t load_texture(const uint8_t *data, uint32_t width, uint32_t height);

#endif //__SYSTEM_H__
