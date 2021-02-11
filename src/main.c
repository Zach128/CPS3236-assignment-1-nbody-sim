#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "cli.h"
#include "fdata.h"
#include "bpoint.h"
#include "nbody.h"

int main(int argc, const char **argv) {
    int  numtasks, rank, len, rc; 
    char hostname[MPI_MAX_PROCESSOR_NAME];

    // Prepare the MPI runtime
    rc = MPI_Init(&argc, &argv);
    if (rc != MPI_SUCCESS) 
    {
        printf ("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        return 1;
    }

    // Register b_point type with mpi.
    const int n_props = 5;
    int blocklengths[5] = { 2, 2, 2, 2, 2 };
    MPI_Datatype types[5] = { MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE };
    MPI_Datatype mpi_b_point_t;
    MPI_Aint offsets[5] = {
        offsetof(b_point, mass),
        offsetof(b_point, vel) + offsetof(vec2, x),
        offsetof(b_point, vel) + offsetof(vec2, y),
        offsetof(b_point, pos) + offsetof(vec2, x),
        offsetof(b_point, pos) + offsetof(vec2, y)
    }; // Byte-wise displacements of each variables/block, measured in int-sized increments.

    MPI_Type_create_struct(n_props, blocklengths, offsets, types, &mpi_b_point_t);
    MPI_Type_commit(&mpi_b_point_t);

    // Acquire info about this process.
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(hostname, &len);

    cli_opt args = process_args(argc, argv);
    double grav_constant = args.grav_constant;
    double time_delta = args.time_step;
    int bodyCount = 0;
    int totalIterations = args.num_iterations;
    b_point *points = NULL;

    // We only want to run this code on the master process.
    if (rank == 0)
    {
        // If the provided file path isn't empty, load data from the file.
        if (args.file_path[0] != '\0') {

            int point_count;
            b_point *file_points = process_file(args.file_path, &point_count);

            printf("Loaded file points:\n");
            print_points(file_points, point_count);

            points = file_points;
            bodyCount = point_count;
        } else if (args.num_particles > 0) {
            int num_points = args.num_particles;
            b_point *rand_points = malloc(num_points * sizeof(b_point));

            generate_rand_points(rand_points, num_points);
            printf("Generated %d points.\n", num_points);
            points = rand_points;
            bodyCount = num_points;
        }
        
        // Initialise a new file first.
        create_new_file(bodyCount);
    }

    // Synchronise data across nodes.
    MPI_Bcast(&bodyCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&grav_constant, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&time_delta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&totalIterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Since slave processes wont have the arrays initialised, tell them to allocate the space.
    if (rank != 0) {
        // int res = MPI_Alloc_mem(sizeof(b_point) * bodyCount, MPI_INFO_ENV, &points);
        points = malloc(sizeof(b_point) * bodyCount);

        // if (res != MPI_SUCCESS) {
        //     printf("Failed to allocate space for points.\n");
        //     return 1;
        // }
    }

    int result = MPI_Bcast(points, bodyCount, mpi_b_point_t, 0, MPI_COMM_WORLD);

    if (result != MPI_SUCCESS) {
        printf("Failed to receive points\n");
        return 1;
    }

    // Debug info.
    // printf("Hello world from process %s with rank %d out of %d processors\n", hostname, rank, numtasks);
    // printf("%d %.f %.f %d\n", bodyCount, grav_constant, time_delta, totalIterations);
    // printf("Size of point array = %ld\n", bodyCount * sizeof(b_point));

    for(int i = 0; i < totalIterations; i++) {
        // Process the points.
        ComputeForces(points, bodyCount, grav_constant, time_delta);
        
        // Output the points (if we're on the master process).
        if (rank == 0)
        {
            save_points_iteration(points, bodyCount);
        }
    }


    // For some reason, running the below would cause one of the nodes to throw a segfault.
    // Clean up
    // if (rank != 0) {
    //     printf("Cleaning up\n");
    //     free(points);
    //     // MPI_Free_mem(points);
    // }

    MPI_Type_free(mpi_b_point_t);
    MPI_Finalize();
    return 0;
}