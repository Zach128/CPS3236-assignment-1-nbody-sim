#include <stdio.h>
#include "cli.h"
#include "fdata.h"
#include "bpoint.h"

int main(int argc, const char **argv) {
    cli_opt args = process_args(argc, argv);
    
    // If the provided file path isn't empty, load data from the file.
    if (args.file_path[0] != '\0') {
        char *str = "./res/data/input_64.txt";

        int point_count;
        b_point *file_points = process_file(str, &point_count);

        printf("Loaded file points:\n");
        print_points(file_points, point_count);
    } else if (args.num_particles > 0) {
        int num_points = args.num_particles;
        b_point points[num_points];

        generate_rand_points(points, num_points);
        printf("Generated %d points.\n", num_points);
    }

    return 0;
}