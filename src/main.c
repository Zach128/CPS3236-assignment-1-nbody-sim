#include <stdio.h>
#include "cli.h"
#include "fdata.h"
#include "../lib/argparse/argparse.h"

int main(int argc, const char **argv) {
    cli_opt args = process_args(argc, argv);
    
    char *str = "./res/data/input_64.txt";

    read_file(str);

    return 0;
}