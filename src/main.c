#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

#include "cli.h"
#include "fdata.h"
#include "bpoint.h"
#include "nbody.h"

int main(int argc, char **argv) {
    int  numtasks, rank, len, rc; 
    char hostname[MPI_MAX_PROCESSOR_NAME];
    struct timespec tstart={0,0}, tend={0,0};

    // Prepare the MPI runtime
    rc = MPI_Init(&argc, &argv);
    if (rc != MPI_SUCCESS) 
    {
        printf ("Error starting MPI program. Terminating.\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        return 1;
    }

    MPI_Datatype mpi_b_point_t = *get_mpi_b_point_type();

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

            if (args.output)
            {
                printf("Loaded file points:\n");
                print_points(file_points, point_count);
            }

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
    }

    // Synchronise data across nodes.
    MPI_Bcast(&bodyCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&grav_constant, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&time_delta, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&totalIterations, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Since slave processes wont have the arrays initialised, tell them to allocate the space.
    if (rank != 0) {
        points = malloc(sizeof(b_point) * bodyCount);
    }

    int result = MPI_Bcast(points, bodyCount, mpi_b_point_t, 0, MPI_COMM_WORLD);

    if (result != MPI_SUCCESS) {
        printf("Failed to receive points\n");
        return 1;
    }

    // Debug info.
    printf("Hello world from process %s with rank %d out of %d processors\n", hostname, rank, numtasks);
    printf("%d %.f %.f %d\n", bodyCount, grav_constant, time_delta, totalIterations);
    printf("Size of point array = %ld\n", bodyCount * sizeof(b_point));

    loadNbodyPoints(numtasks, rank, points, bodyCount, time_delta);

    // Get the start time of the calculation.
    if (rank == 0) {
        clock_gettime(CLOCK_MONOTONIC, &tstart);
    }

    for (int i = 0; i < totalIterations; i++) {
        int syncResult = MPI_Bcast(points, bodyCount, mpi_b_point_t, 0, MPI_COMM_WORLD);

        if (syncResult != MPI_SUCCESS) {
            printf("Failed to broadcast points in iteration %d\n", i);
            return 1;
        }
        // Process the points.
        ComputeForces(points, bodyCount, grav_constant, time_delta);

        //Synchronise the points buffer across all nodes.
        syncBodiesWithMaster(points);

        // Output the points (if we're on the master process).
        if (rank == 0 && args.output)
        {
            save_points_iteration(points, bodyCount, i + 1);
        }
    }

    // MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Simulation complete. Final body arrangement:\n");
        print_points(points, bodyCount);
        // Get the end-time and print the duration of the simulation.
        clock_gettime(CLOCK_MONOTONIC, &tend);
        printf("Nbody simulation took about %.5f seconds\n",
           ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - 
           ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
    }

    // For some reason, running the below would cause one of the nodes to throw a segfault.
    // Clean up
    // if (rank != 0) {
    //     printf("Cleaning up\n");
    //     free(points);
    //     // MPI_Free_mem(points);
    // }

    // MPI_Type_free(&mpi_b_point_t);
    MPI_Finalize();
    return 0;
}