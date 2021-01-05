#ifndef TYPES_H_
#define TYPES_H_

typedef struct {
    double x;
    double y;
} vector_2d;
typedef vector_2d vec2;

typedef struct {
    double mass;
    vec2 pos;
} body_point;
typedef body_point b_point;

#endif