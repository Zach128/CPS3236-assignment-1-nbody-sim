#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> 

#include "vector2.h"
#include "bpoint.h"

MPI_Datatype mpi_b_point_t;

b_point *point_send_buffer = NULL;

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
		index_froms = malloc(sizeof(int) * totalNodes);
		index_tos = malloc(sizeof(int) * totalNodes);
		counts = malloc(sizeof(int) * totalNodes);

		mpi_b_point_t = *get_mpi_b_point_type();

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

void syncBodiesWithMaster(b_point *points)
{
	b_point sendBuff[count];

	MPI_Barrier(MPI_COMM_WORLD);
	if (_rank != 0)
	{

		// If running in a slave processor, prepare a point buffer to send over with the relavant data.
		for(int i = 0; i < count; i++)
		{
			sendBuff[i] = points[index_body_from + i];
		}

		// printf("Rank %d: Sending %d elements with offset of %d\n", _rank, count, index_body_from);
		MPI_Send(sendBuff, count, mpi_b_point_t, 0, 0, MPI_COMM_WORLD);
	}
	else
	{
		// If running in the master processor, begin gathering all the new point data submitted by the slave processors.
		for(int i = 1; i < total_nodes; i++)
		{
			b_point recBuff[counts[i]];
			MPI_Recv(recBuff, counts[i], mpi_b_point_t, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			// Write the new data to the points array.
			for(int recI = 0; recI < counts[i]; recI++)
			{
				points[index_froms[i] + recI] = recBuff[recI];
			}
		}

		// printf("Rank %d: Synchonised point data\n", _rank);
	}

	// int result = MPI_Allgatherv(sendBuff, count, mpi_b_point_t, recvBuff, counts, index_froms, mpi_b_point_t, MPI_COMM_WORLD);
}

void MoveBodies(b_point *bodies, int body_count, float time_delta)
{
	vec2 buffer = { 0.f, 0.f };

#ifdef USE_OMP
	#pragma omp parallel for private(buffer), schedule(static, 8)
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
	#pragma omp parallel for private(direction,force,acceleration,buffer, distance), schedule(static, 8)
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
