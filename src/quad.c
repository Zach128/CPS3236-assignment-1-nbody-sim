#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "quad.h"
#include "bpoint.h"

b_node *init_empty_node()
{
    b_node *node = malloc (sizeof(b_node));
    *node = (b_node){
        .children = { NULL, NULL, NULL, NULL },
        .points = { NULL, NULL, NULL, NULL },
        .child_count = 0,
        .parent = NULL,
        .mass = 0,
        .center_of_mass = { 0, 0 },
        .boundary = { { 0, 0 }, { 0, 0} }
    };

    return node;
}

void free_node(b_node *node)
{
    for(int i = 0; i < BARNES_MAX_NODES; i++)
    {
        if (node->children[i] != NULL)
            free_node(node->children[i]);
    }

    free(node);
}

void init_child_bounding_box(b_node *root, int index)
{
    vec2 center = { 0, 0 };

    // We assume a uniform 2x2 square subdivision, so we can assume it will be half-sized.
    vec2 halfSize = (vec2){
        .x = root->boundary.half_size.x / 2,
        .y = root->boundary.half_size.y / 2,
    };

    // Top-left
    if (index == 0)
    {
        center = (vec2){
            .x = root->boundary.center.x - halfSize.x,
            .y = root->boundary.center.y + halfSize.y,
        };
    }
    // Top-right
    else if (index == 1)
    {
        center = (vec2){
            .x = root->boundary.center.x + halfSize.x,
            .y = root->boundary.center.y + halfSize.y,
        };
    }
    // Bottom-left
    else if (index == 2)
    {
        center = (vec2){
            .x = root->boundary.center.x - halfSize.x,
            .y = root->boundary.center.y - halfSize.y,
        };
    }
    // Bottom-right
    else if (index == 3)
    {
        center = (vec2){
            .x = root->boundary.center.x + halfSize.x,
            .y = root->boundary.center.y - halfSize.y,
        };
    }

    root->children[index]->boundary.center = center;
    root->children[index]->boundary.half_size = halfSize;
}

bool is_inside_box(vec2 *coord, aabb *box)
{
    // Get the 2d distance between the target and the center of the box.
    vec2 dist = {
        .x = fabs(coord->x - box->center.x),
        .y = fabs(coord->y - box->center.y)
    };

    // If the distance is smaller than the half-size, we're in the box.
    if (dist.x <= box->half_size.x && dist.y <= box->half_size.y)
        return true;
    
    return false;
}

b_node *create_root_node(b_point *points, int pointCount)
{
    b_node *root = init_empty_node();
    
    // Bottom-left corner
    vec2 a = {.x = 0, .y = 0};
    // Top right corner
    vec2 b = {.x = 0, .y = 0};

    // Find the bounding box of the root by finding the farthest points.
    for (int i = 0; i < pointCount; i++)
    { 
        // If any point is farther than our farthest a or b, copy them for our bounding box.
        if (points[i].pos.x < a.x) a.x = points[i].pos.x;
        if (points[i].pos.y < a.y) a.y = points[i].pos.y;
        if (points[i].pos.x > b.x) b.x = points[i].pos.x;
        if (points[i].pos.y > b.y) b.y = points[i].pos.y;
    }

    vec2 halfSize = {
        .x = ceil(fabs((b.x - a.x) / 2)),
        .y = ceil(fabs((b.y - a.y) / 2)),
    };
    
    vec2 center = {
        .x = halfSize.x + a.x,
        .y = halfSize.y + a.y,
    };

    // Record the bounding box.
    aabb boundary = {
        .center = center,
        .half_size = halfSize
    };
    root->boundary = boundary;

    return root;
}

void subdivide(b_node *root)
{

    // Initialise each child node.
	for(int i = 0; i < BARNES_MAX_NODES; i++)
	{
        // Create and assign the child node.
		b_node *child = init_empty_node();
        child->parent = root;
		root->children[i] = child;

        // Now that the child node is connected, get the bounding box.
        init_child_bounding_box(root, i);
	}

    for(int i = 0; i < BARNES_MAX_NODES; i++)
    {
        // If the point is not null, try insert it into the child nodes.
        if (root->points[i] != NULL)
        {
            for(int j = 0; j < BARNES_MAX_NODES; j++)
            {
                // If the point inserts successfully, remove its address from the root and break.
                if (insert(root->points[i], root->children[j]))
                {
                    root->points[i] = NULL;
                    break;
                }
            }
        }
    }
}

bool insert(b_point *point, b_node *root_node)
{
    if (!is_inside_box(&point->pos, &root_node->boundary))
        return false;

    // First, check we already have the point.
    // Will be replaced by a space query.
	for (int i = 0; i < root_node->child_count; i++)
	{
		// If this node has the same point already, we can't add it.
		if (root_node->points[i] == point)
		{
			return false;
		}
	}

	// If there are fewer points than the limit and the first node is uninitialized, store it as a child.
	if(root_node->child_count < BARNES_MAX_NODES && root_node->children[0] == NULL)
	{
        // Append the new point to the array.
		root_node->points[root_node->child_count] = point;
        root_node->child_count += 1;

		return true;
	}
    // Otherwise, if the nodes are yet uninitialised, subdivide 
    else if (root_node->children[0] == NULL)
	{
        subdivide(root_node);
	}

    // Try to insert the point into the child nodes.
    for(int i = 0; i < BARNES_MAX_NODES; i++)
    {
        if (insert(point, root_node->children[i])) return true;
    }

    // We shouldn't reach this spot. If we do, something went wrong.
    return false;
}

b_node *tree_from_points(b_point *points, int bodyCount)
{
    b_node *tree = create_root_node(points, bodyCount);

	for(int i = 0; i < bodyCount; i++)
	{
		insert(&(points[i]), tree);
	}

    return tree;
}
