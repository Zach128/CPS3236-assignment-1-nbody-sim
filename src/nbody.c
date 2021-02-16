#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 

#include "vector2.h"
#include "bpoint.h"

// Type of the b_point struct.
MPI_Datatype mpi_b_point_t;
// Buffer for sending points to the master processor.
b_point *point_send_buffer = NULL;
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
		index_froms = calloc(totalNodes, sizeof(int));
		index_tos = calloc(totalNodes, sizeof(int));
		counts = calloc(totalNodes, sizeof(int));

		mpi_b_point_t = get_mpi_b_point_type();

		total_nodes = totalNodes;
		_rank = rank;
		body_count = bodyCount;
		// Compute each processors personal variables pertaining to the chunk of points it shall compute.
		index_body_from = body_count / totalNodes * rank;
		index_body_to = rank == totalNodes - 1 ? body_count - 1 : body_count / totalNodes * (rank + 1) - 1;
		count = index_body_to - index_body_from + 1;
		point_send_buffer = calloc(count, sizeof(b_point));

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
	b_point *sendBuff = malloc(count * sizeof(b_point));

	// Prepare a buffer with this ranks points to be sent.
	for(int i = 0; i < count; i++)
	{
		sendBuff[i] = points[index_body_from + i];
	}

	// Dispatch the data, writing it back to the points array.
	// Location in the array and how much to receive is determined by the sending rank.
	MPI_Allgatherv(sendBuff, count, mpi_b_point_t, points, counts, index_froms, mpi_b_point_t, MPI_COMM_WORLD);
	
	free(sendBuff);
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
		vec2Mulf(&bodies[i].vel, time_delta, &buffer);
		vec2Addvec2(&bodies[i].pos, &buffer, &bodies[i].pos);
	}
}

void ComputeForces(b_point *bodies, int body_count, float grav_constant, float time_delta)
{
	vec2 direction,
		force, acceleration, buffer;

	float distance;

#ifdef USE_OMP
	#pragma omp parallel for private(direction,force,acceleration,buffer, distance), schedule(static)
#endif
		for (int j = index_body_from; j <= index_body_to; j++)
		{
			b_point *p1 = &bodies[j];
		
			force.x = acceleration.x = buffer.x = 0.f;
			force.y = acceleration.y = buffer.y = 0.f;
		
			for (int k = 0; k < body_count; k++)
			{
				if (k == j) continue;
			
				b_point *p2 = &bodies[k];
				
				// Compute direction vector.
				direction.x = p2->pos.x - p1->pos.x;
				direction.y = p2->pos.y - p1->pos.y;

				// Calculate the length of the computed direction.
				double direction_length = vec2Length(&direction);
				
				// Limit distance term to avoid singularities.
				// distance = std::max<float>( 0.5f * (p2.Mass + p1.Mass), fabs(direction.Length()) );
				distance = 0.5f * (p2->mass + p1->mass);
				distance = distance >= direction_length ? distance : direction_length;
				
				// Accumulate force. Follows the below calculation
				// force += direction / (distance * distance * distance) * p2->mass; 
				vec2Divf(&direction, (distance * distance * distance), &force);
				vec2Mulf(&force, p2->mass, &force);
			}
					
			// Compute acceleration for body.
			// acceleration = force * p_gravitationalTerm;
			vec2Mulf(&force, grav_constant, &acceleration);

			// Integrate velocity (m/s).
			// p1.Velocity += acceleration * p_deltaT;
			vec2Addvec2(&p1->vel, vec2Mulf(&acceleration, time_delta, &buffer), &p1->vel);
		}

	MoveBodies(bodies, body_count, time_delta);

}
