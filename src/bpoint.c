#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include <mpi.h>

#include "bpoint.h"

MPI_Datatype mpi_type = 0;

MPI_Datatype get_mpi_b_point_type() {
    if (mpi_type == 0) {
        // Register b_point type with mpi.
        const int n_props = 5;
        int blocklengths[5] = { 2, 2, 2, 2, 2 };
        MPI_Datatype types[5] = { MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE };
        MPI_Aint offsets[5] = {
            offsetof(b_point, mass),
            offsetof(b_point, vel) + offsetof(vec2, x),
            offsetof(b_point, vel) + offsetof(vec2, y),
            offsetof(b_point, pos) + offsetof(vec2, x),
            offsetof(b_point, pos) + offsetof(vec2, y)
        }; // Byte-wise displacements of each variables/block, measured in int-sized increments.

        MPI_Type_create_struct(n_props, blocklengths, offsets, types, &mpi_type);
        MPI_Type_commit(&mpi_type);

        return mpi_type;
    } else {
        return mpi_type;
    }
}

void print_point(b_point *point) {
    printf("m: %.3lf v: %.3lf, %.3lf p: %.3lf, %.3lf\n", point->mass, point->vel.x, point->vel.y, point->pos.x, point->pos.y);
}

void print_points(b_point *points, const int count) {
    for(int i = 0; i < count; i++) {
        printf("%d: ", i);
        print_point(&points[i]);
    }
}

void generate_rand_points(b_point *points, const int count) {
    // Set a new seed.
    srand(time(NULL));

    // Generate random ranges of values for the mass, x, and y.
    for (int i = 0; i < count; i++) {
        points[i].mass = (double) rand() / RAND_MAX * MASS_RAND_MAX;
        points[i].vel.x = 0.f;
        points[i].vel.y = 0.f;
        points[i].pos.x = (double) X_RAND_MIN + rand() / (RAND_MAX / (X_RAND_MAX - X_RAND_MIN + 1.0) + 1.0);
        points[i].pos.y = (double) Y_RAND_MIN + rand() / (RAND_MAX / (Y_RAND_MAX - Y_RAND_MIN + 1.0) + 1.0);
    }
}