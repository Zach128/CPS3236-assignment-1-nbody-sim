#include <stdio.h>
#include "cli.h"
#include "fdata.h"
#include "bpoint.h"
#include "../lib/argparse/argparse.h"

int main(int argc, const char **argv) {
    cli_opt args = process_args(argc, argv);
    
    char *str = "./res/data/input_64.txt";

    process_file(str);

    b_point points[10];

    generate_rand_points(points, 10);

    printf("Generated points:\n");
    print_points(points, 10);

    return 0;
}