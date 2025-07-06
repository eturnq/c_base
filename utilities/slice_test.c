#include "slice_test.h"
#include "../utilities.h"

TestResult *slice_compare(TestResult *result) {
	INIT_RESULT(result, "[slice_compare]");

	int int1 = 1;
	int int2 = 2;
	Slice s1 = { &int1, sizeof(int) };
	Slice s2 = { &int2, sizeof(int) };

	if (slice_cmp(s1, s1) != 0) {
		MSG_PRINT(result, " slice_cmp should return 0");
		return result;
	}

	if (slice_cmp(s1, s2) != -1) {
		MSG_PRINT(result, " slice_cmp should return -1");
		return result;
	}

	if (slice_cmp(s2, s1) != 1) {
		MSG_PRINT(result, " slice_cmp should return 1");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *slice_sub_test(TestResult *result) {
	Slice s1, s2;
	char test_data[20];
	INIT_RESULT(result, "[slice_sub]");

	sprintf(test_data, "This is a test");
	s1.data = test_data;
	s1.length = 14;

	s2 = slice_sub(s1, 5, 9);

	if (s2.length != 9) {
		MSG_PRINT(result, " s2 has the wrong length");
		return result;
	}

	if (strncmp("is a test", s2.data, s2.length) != 0) {
		MSG_PRINT(result, " s2 has the wrong data");
		return result;
	}

	result->status = TEST_PASS;
	return result;
}

TestResult *slice_copy_test(TestResult *result) {
    char test_data1[20];
    char test_data2[20];
    INIT_RESULT(result, "[slice_copy_test]");

    sprintf(test_data1, "This is a test");
    Slice s1 = { test_data1, 14 };
    Slice s2 = { test_data2, 14 };

    Result res = slice_copy(s2, s1);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to perform slice copy");
        return result;
    }

    if (slice_cmp(s1, s2) != 0) {
        MSG_PRINT(result, " Slices are not equivalent");
        return result;
    }

    result->status = TEST_PASS;
    return result;
}
