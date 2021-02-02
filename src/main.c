#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "fdata.h"
#include "bpoint.h"
#include "nbody.h"

int main(int argc, const char **argv) {
    cli_opt args = process_args(argc, argv);
    double grav_constant = args.grav_constant;
    double time_delta = args.time_step;
    int bodyCount = 0;
    b_point *points = NULL;

    // If the provided file path isn't empty, load data from the file.
    if (args.file_path[0] != '\0') {
        char *str = "./res/data/input_64.txt";

        int point_count;
        b_point *file_points = process_file(str, &point_count);

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

    // Process the points.
    print_point(&points[0]);
    ComputeForces(points, bodyCount, grav_constant, time_delta);
    print_point(&points[0]);
    MoveBodies(points, bodyCount, time_delta);
    print_point(&points[0]);

    // Output the points.
    save_points_to_file(points, bodyCount);

    return 0;
}