#ifndef VECTOR_2_H_
#define VECTOR_2_H_

#include <math.h>

typedef struct vec2 {
    double x;
    double y;
} vec2;

typedef struct mvec2 {
    double m;
    vec2 v;
} mvec2;

// Get the length of a given vector.
#define VEC2_LENGTH(v) (fabs(sqrtf(v.x * v.x + v.y * v.y)))

#endif