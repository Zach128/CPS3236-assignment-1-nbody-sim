#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#include "vector2.h"
#include "nbody.h"
#include "bpoint.h"
#include "quad.h"
#include "barnes.h"

b_point *sendBuff = NULL;
// Type of the b_point struct.
MPI_Datatype mpi_b_point_t;
// Prevent dupplicate initialisations.
bool isInitialised = false;
// How many processors are active.
int total_nodes;
// How many points are covered by the simulator.
int body_count;
// The processors rank.
int _rank;
// How many points the processor tracks.
int count;
// How many points each processor is responsible for.
int *counts;
// Starting index for a processors chunk of points.
int *index_froms;
// Ending index for a processors chunk of points.
int *index_tos;
// Processors own start and end index.
int index_body_from, index_body_to;

void loadNbodyPoints(int totalNodes, int rank, b_point *bodies, int bodyCount, float time_delta)
{
	// we don't want to run this more than once.
	if (!isInitialised)
	{
		// index_froms = malloc(totalNodes * sizeof(int));
		// index_tos = malloc(totalNodes * sizeof(int));
		// counts = malloc(totalNodes * sizeof(int));
		MPI_Alloc_mem(totalNodes * sizeof(int), MPI_INFO_NULL, &index_froms);
		MPI_Alloc_mem(totalNodes * sizeof(int), MPI_INFO_NULL, &index_tos);
		MPI_Alloc_mem(totalNodes * sizeof(int), MPI_INFO_NULL, &counts);

		mpi_b_point_t = get_mpi_b_point_type();

		total_nodes = totalNodes;
		_rank = rank;
		body_count = bodyCount;
		// Compute each processors personal variables pertaining to the chunk of points it shall compute.
		index_body_from = body_count / totalNodes * rank;
		index_body_to = rank == totalNodes - 1 ? body_count - 1 : body_count / totalNodes * (rank + 1) - 1;
		count = index_body_to - index_body_from + 1;

		MPI_Alloc_mem(count * sizeof(b_point), MPI_INFO_NULL, &sendBuff);

		// Synchronize all computed chunk data across all points.
		MPI_Allgather(&index_body_from, 1, MPI_INT, index_froms, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Allgather(&index_body_to, 1, MPI_INT, index_tos, 1, MPI_INT, MPI_COMM_WORLD);
		MPI_Allgather(&count, 1, MPI_INT, counts, 1, MPI_INT, MPI_COMM_WORLD);
		
		printf("Rank %d shall process points from %d to %d\n", rank, index_body_from, index_body_to);

		isInitialised = true;
	}
}

void syncBodiesAcrossRanks(b_point *points)
{
	// b_point *sendBuff = malloc(count * sizeof(b_point));
	// MPI_Alloc_mem(count * sizeof(b_point), MPI_INFO_NULL, &sendBuff);

	// Prepare a buffer with this ranks points to be sent.
	for(int i = 0; i < count; i++)
	{
		sendBuff[i] = points[index_body_from + i];
	}

	// Dispatch the data, writing it back to the points array.
	// Location in the array and how much to receive is determined by the sending rank.
	MPI_Allgatherv(sendBuff, count, mpi_b_point_t, points, counts, index_froms, mpi_b_point_t, MPI_COMM_WORLD);
	
	// MPI_Free_mem(sendBuff);
}

void MoveBodies(b_point *bodies, int body_count, float time_delta)
{
	vec2 buffer = { 0.f, 0.f };

#ifdef USE_OMP
	#pragma omp parallel for private(buffer), schedule(static)
#endif
	for (int i = index_body_from; i <= index_body_to; i++)
	{
		// bodies[i]->pos += bodies[i].vel * time_delta;
		// vec2Mulf(&bodies[i].vel, time_delta, &buffer);
		// vec2Addvec2(&bodies[i].pos, &buffer, &bodies[i].pos);
		
		buffer.x += bodies[i].vel.x * time_delta;
		buffer.y += bodies[i].vel.y * time_delta;

		bodies[i].pos.x += buffer.x;
		bodies[i].pos.y += buffer.y;
	}
}

void get_force(b_point *target, vec2 *src, double srcMass, vec2 *out)
{
	float distance;
	vec2 direction;

	// Compute direction vector.
	direction.x = src->x - target->pos.x;
	direction.y = src->y - target->pos.y;

	// Calculate the length of the computed direction.
	double direction_length = fabs(vec2Length(&direction));
	
	// Limit distance term to avoid singularities.
	// distance = std::max<float>( 0.5f * (p2.Mass + p1.Mass), fabs(direction.Length()) );
	distance = 0.5f * (srcMass + target->mass);
	distance = distance >= direction_length ? distance : direction_length;

	// Accumulate force. Follows the below calculation
	// force += direction / (distance * distance * distance) * p2->mass; 
	// vec2Divf(&direction, (distance * distance * distance), out);
	// vec2Mulf(out, srcMass, out);
	distance = distance * distance * distance;
	out->x += direction.x / distance * srcMass;
	out->y += direction.y / distance * srcMass;
}

void compute_velocity(b_point *target, vec2 *force, float grav_constant, float time_delta)
{
	vec2 acceleration = {0, 0};
	vec2 buffer = {0, 0};

	// Compute acceleration for body.
	// acceleration = force * p_gravitationalTerm;
	// vec2Mulf(force, grav_constant, &acceleration);
	acceleration.x = force->x * grav_constant;
	acceleration.y = force->y * grav_constant;

	// Integrate velocity (m/s).
	// p1.Velocity += acceleration * p_deltaT;
	// vec2Addvec2(&target->vel, vec2Mulf(&acceleration, time_delta, &buffer), &target->vel);
	target->vel.x += acceleration.x * time_delta;
	target->vel.y += acceleration.y * time_delta;
}

void ComputeForces(b_point *bodies, int body_count, float grav_constant, float time_delta)
{
	vec2 force = { 0, 0 };
	vec2 acceleration = { 0, 0 };
	vec2 buffer = { 0, 0 };

#ifdef USE_OMP
	#pragma omp parallel for private(force,acceleration,buffer), schedule(static)
#endif
		for (int j = index_body_from; j <= index_body_to; j++)
		{
			b_point *p1 = &bodies[j];
		
			force.x = acceleration.x = buffer.x = 0.f;
			force.y = acceleration.y = buffer.y = 0.f;

			if (j == 13)
			{
				// printf("Here\n");
			}
		
			for (int k = 0; k < body_count; k++)
			{
				if (k == j) continue;
				if (k == 14)
				{
					// printf("Here\n");
				}

				b_point *p2 = &bodies[k];
				
				get_force(p1, &p2->pos, p2->mass, &force);
			}

			compute_velocity(p1, &force, grav_constant, time_delta);
		}

	MoveBodies(bodies, body_count, time_delta);

}

bool is_node_close_enough(b_point *target, b_node *tree)
{
	vec2 fullSize = { .x = tree->boundary.half_size.x * 2, .y = tree->boundary.half_size.y * 2 };
	vec2 d = { .x = fabs(target->pos.x - tree->center_of_mass.v.x), .y = fabs(target->pos.y - tree->center_of_mass.v.y) };
	double s = (fullSize.x + fullSize.y) / 2;
	double qoutient = s / vec2Length(&d);

	return qoutient > BARNES_NODE_DIST_TITA;
}

void barnesComputeForce(b_point *target, b_node *tree, vec2 *force)
{

	for (int i = 0; i < tree->child_count; i++)
	{
		if (tree->children[i] != NULL)
		{
			// If there are child nodes, first check if it's close enough from the target point.
			if (is_node_close_enough(target, tree->children[i]))
			{
				// If it's close enough, we recursively iterate over the child node.
				barnesComputeForce(target, tree->children[i], force);
			}
			else
			{
				// Otherwise
				get_force(target, &tree->children[i]->center_of_mass.v, tree->children[i]->center_of_mass.m, force);
			}
		}
		else
		{
			for (int i = 0; i < tree->child_count; i++)
			{
				if (target == tree->points[i]) continue;

				get_force(target, &tree->points[i]->pos, tree->points[i]->mass, force);
			}
		}
	}
}

void BarnesMain(b_point *points, int bodyCount, float grav_constant, float time_delta)
{

	b_node *tree = tree_from_points(points, bodyCount);

	for(int i = 0; i < bodyCount; i++)
	{
		vec2 force = { 0, 0 };
		barnesComputeForce(&points[i], tree, &force);

		compute_velocity(&points[i], &force, grav_constant, time_delta);
	}

	MoveBodies(points, body_count, time_delta);

	free_node(tree);
}
