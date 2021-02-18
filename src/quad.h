#ifndef QUAD_H_
#define QUAD_H_
#include <stdbool.h>

#include "vector2.h"
#include "bpoint.h"

#define BARNES_NODE_DIST_TITA 0.5
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
    mvec2 center_of_mass;
    int child_count;
    aabb boundary;
    struct b_node *parent;
} b_node;

/**
 * Function: create_root_node
 * ---------------------
 * Create a root node, encompassing the entire point space.
 * 
 * points: Array of bodies to check.
 * pointCount: The number of bodies in the array.
 * 
 * returns: The newly-created root node.
 **/
b_node *create_root_node(b_point *points, int pointCount);

/**
 * Function: free_node
 * ---------------------
 * Destroy the given node and all child nodes recursively.
 **/
void free_node(b_node *node);

/**
 * Function: insert
 * ---------------------
 * Insert a new body into the given node. If the node does not have space for the point, it will subdivide and recursively try to insert into the sub-nodes.
 * If the point lies outside the boundary of the node, it will not be inserted.
 * 
 * point: The point to insert.
 * root_node: The root node to insert into.
 * 
 * returns: Whether the insert was successful or not.
 **/
bool insert(b_point *point, b_node *root_node);

/**
 * Function: tree_from_points
 * ---------------------
 * points: Array of bodies to populate into the tree.
 * bodyCount: The number of bodies in the array.
 * 
 * returns: the fully-constructed tree.
 **/
b_node *tree_from_points(b_point *points, int bodyCount);

/**
 * Function: subdivide
 * ---------------------
 * Subdivide the given root node into 4 child nodes.
 * 
 * root: the given root node to subdivide.
 **/
void subdivide(b_node *root);

#endif
