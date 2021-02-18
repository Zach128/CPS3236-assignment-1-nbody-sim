#include <math.h>
#include "vector2.h"

// **+** //

vec2 *vec2Addf(vec2 *v1, float value, vec2 *out)
{
    out->x = v1->x + value;
    out->y = v1->y + value;

    return out;
}

vec2 *vec2Addvec2(vec2 *v1, vec2 *v2, vec2 *out)
{
    out->x = v1->x + v2->x;
    out->y = v1->y + v2->y;

    return out;
}

// ***** //

// **/** //

vec2 *vec2Divf(vec2 *v1, float value, vec2 *out)
{
    out->x = v1->x / value;
    out->y = v1->y / value;

    return out;
}

vec2 *vec2Divvec2(vec2 *v1, vec2 *v2, vec2 *out)
{
    out->x = v1->x / v2->x;
    out->y = v1->y / v2->y;

    return out;
}

// ***** //

// ***** //

vec2 *vec2Mulf(vec2 *v1, float value, vec2 *out)
{
    out->x = v1->x * value;
    out->y = v1->y * value;

    return out;
}

vec2 *vec2Mulvec2(vec2 *v1, vec2 *v2, vec2 *out)
{
    out->x = v1->x * v2->x;
    out->y = v1->y * v2->y;

    return out;
}

// ***** //

double vec2Length(vec2 *v)
{
    return sqrtf(v->x * v->x + v->y * v->y);
}
