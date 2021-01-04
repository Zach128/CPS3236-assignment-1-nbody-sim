#include <stdio.h>
#include "cli.h"
#include "../lib/argparse/argparse.h"

int
main(int argc, const char **argv)
{
    struct argparse argparse = get_argparse();
    argc = argparse_parse(&argparse, argc, argv);
    return 0;
}