#include "utils.h"

#include <string.h>
#include <ctype.h>

void replace_char(char* str, char find, char replace) {
    char *current_pos = strchr(str,find);
    for (char* p = current_pos; (current_pos = strchr(str, find)) != NULL; *current_pos = replace);
}

void to_uppercase(char * str) {
    char *s = str;
    while (*s) {
        *s = toupper((unsigned char) *s);
        s++;
    }
}
