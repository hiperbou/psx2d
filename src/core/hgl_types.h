#ifndef _HGL_TYPES_H_
#define _HGL_TYPES_H_

#include <stddef.h>

typedef int fix32;
typedef int u16;
typedef int s16;

#define FIXED_POINT_UNIT 4096 // 2 << 12
#define FIXED_POINT_SHIFT 12

#define FIX32(X) ((int)((X) * FIXED_POINT_UNIT))
#define fix32ToInt(X) ((X) >> FIXED_POINT_SHIFT)

//void (*functionName)(void);
#define fun(X) void (*X)(void)
#define fun2(X,Y) void (*X)(Y)
#define fun3(X,Y,Z) X (*Y)(Z)


#endif //_HGL_TYPES_H_