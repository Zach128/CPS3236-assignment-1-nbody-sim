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

/**
 * Function: process_args
 * -------------------
 * Take the string inputs from the command line and map them to our args object.
 * 
 * argc: The argc variable passed through main().
 * argv: The argv variable passed through main().
 * 
 * returns: A cli_opt object containing all the parsed arguments.
 * 
 **/
cli_opt process_args(int argc, char **argv);

#endif
