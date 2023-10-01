#include <string.h>

#define FONT_CHAR_SIZE 8

int text_size(const char* text) {
    return strlen(text) * FONT_CHAR_SIZE;
}

int text_get_centered_position(const char* text, int x) {
    int halfTextSize = text_size(text) >> 1;
    return x - halfTextSize;
}

