#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "test_fdata.h"

int main(int argc, char *argv[]) {

    cmocka_run_group_tests_name("fdata", fdata_tests, NULL, NULL);

    return 0;
}
