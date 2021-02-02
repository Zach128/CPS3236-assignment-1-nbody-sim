#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "bpoint.h"

void print_point(b_point *point) {
    printf("m: %.3lf v: %.3lf, %.3lf p: %.3lf, %.3lf\n", point->mass, point->vel.x, point->vel.y, point->pos.x, point->pos.y);
}

void print_points(b_point *points, const int count) {
    for(int i = 0; i < count; i++) {
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