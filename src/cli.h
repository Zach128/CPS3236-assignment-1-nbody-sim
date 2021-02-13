#ifndef CLI_H_
#define CLI_H_

#include <stdbool.h>

struct _nbody_options {
    char *file_path;
    bool output;
    int num_particles;
    double grav_constant;
    int num_iterations;
    double time_step;
};

typedef struct _nbody_options cli_opt;

cli_opt process_args(int argc, char **argv);

#endif
