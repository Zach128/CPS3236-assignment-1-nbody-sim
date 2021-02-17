#ifndef QUAD_H_
#define QUAD_H_

#include "vector2.h"
#include "bpoint.h"

#define BARNES_MAX_NODES 4

typedef struct aabb {
    vec2 center;
    // Radius from center per-axis
    vec2 half_size;
} aabb;

typedef struct b_node {
    // 0 = nw, 1 = ne, 2 = sw, 3 = se
    b_point *points[BARNES_MAX_NODES];
    struct b_node *children[BARNES_MAX_NODES];
    vec2 center_of_mass;
    double mass;
    int child_count;
    aabb boundary;
    struct b_node *parent;
} b_node;

b_node *create_root_node(b_point *points, int pointCount);
bool insert(b_point *point, b_node *root_node);

#endif
