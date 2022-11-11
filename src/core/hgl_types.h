#ifndef _HGL_TYPES_H_
#define _HGL_TYPES_H_

#include <stddef.h>

typedef int fix32;
typedef int u16;
typedef int s16;

#define FIXED_POINT_UNIT 4096 // 2 ^ 12
#define FIXED_POINT_SHIFT 12    // 1 << 12 = 4096

#define FIX32(X) ((int)((X) * FIXED_POINT_UNIT))
#define fix32ToInt(X) ((X) >> FIXED_POINT_SHIFT)

//void (*functionName)(void);
#define fun(X) void (*X)(void)
#define fun2(X,Y) void (*X)(Y)
#define fun3(X,Y,Z) X (*Y)(Z)



#define MIN(X,Y) ((X)<(Y)) ? (X) : (Y)
#define MAX(X,Y) ((X)>(Y)) ? (X) : (Y)

#define CLAMP(X, A, B) MAX((A), ( MIN((X),(B)) ))

//interp = (a*(256 - x) + b*x) >> 8
#define LinearInterpolate(X,Y,Z,UNIT) (X*(UNIT-Z)+Y*Z)
#define LinearInterpolateFIX32(X,Y,Z) ((X)*(FIX32(1)-(Z))+(Y)*(Z))

inline fix32 linearInterpolateFix32(int a, int b, fix32 t) {
    return (a*(FIX32(1)-t)+b*t);
}

#endif //_HGL_TYPES_H_
