#include <stdio.h>
#include "cli.h"
#include "../lib/argparse/argparse.h"

int
main(int argc, const char **argv)
{
    cli_opt args = process_args(argc, argv);
    
    return 0;
}