#ifndef TEST_FDATA_H_
#define TEST_FDATA_H_

void test_open_file_fails_bad_path(void **state);
void test_open_file_success(void **state);

const struct CMUnitTest fdata_tests[] = {
    cmocka_unit_test(test_open_file_fails_bad_path),
    cmocka_unit_test(test_open_file_success)
};

#endif