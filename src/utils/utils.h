#ifndef DEMO_UTILS_H
#define DEMO_UTILS_H

#define print(X) printf(X"\n")
#define println(X, ...) printf(X"\n", __VA_ARGS__)


void replace_char(char* str, char find, char replace);
void to_uppercase(char * str);

#endif //DEMO_UTILS_H
