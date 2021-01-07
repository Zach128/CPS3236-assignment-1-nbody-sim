#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

#include "../src/fdata.h"

char * realpath(const char *path) {
    return (char *) mock();
}

// Mock the fopen method.
FILE * fopen(const char *__restrict __filename, const char *__restrict __modes) {
  return mock_ptr_type(FILE *);
}

void test_open_file_fails_bad_path(void **state) {
    will_return(realpath, NULL);
    assert_null(open_file("abc"));
}

void test_open_file_success(void **state) {
    will_return(realpath, "tests.out");
    will_return(fopen, "qests");
    assert_non_null(open_file("tests.out"));
}
