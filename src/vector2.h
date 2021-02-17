#ifndef VECTOR_2_H_
#define VECTOR_2_H_

typedef struct vec2 {
    double x;
    double y;
} vec2;

typedef struct mvec2 {
    double m;
    vec2 v;
} mvec2;

vec2 *vec2Addvec2(vec2 *v1, vec2 *v2, vec2 *out);
vec2 *vec2Addf(vec2 *v1, float value, vec2 *out);

vec2 *vec2Divvec2(vec2 *v1, vec2 *v2, vec2 *out);
vec2 *vec2Divf(vec2 *v1, float value, vec2 *out);

vec2 *vec2Mulvec2(vec2 *v1, vec2 *v2, vec2 *out);
vec2 *vec2Mulf(vec2 *v1, float value, vec2 *out);

double vec2Length(vec2 *v);

#endif