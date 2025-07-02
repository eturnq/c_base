#include "arraylist_test.h"
#include "../utilities.h"

TestResult *array_list_init_deinit(TestResult *result) {
    INIT_RESULT(result, "[array_list_init_deinit]");

    Allocator *heap = get_raw_heap_allocator();
    Result res = new_array_list(heap, sizeof(int), 16);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to create new ArrayList");
        return result;
    }

    ArrayList al = *((ArrayList*)res.data.data);

    res = deinit_array_list(&al);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to deinit ArrayList");
        return result;
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *array_list_push_pop(TestResult *result) {
    INIT_RESULT(result, "[array_list_push_pop]");

    Allocator *heap = get_raw_heap_allocator();
    Result res = new_array_list(heap, sizeof(int), 16);
    ArrayList al = *((ArrayList*)res.data.data);
    int int1 = 1;
    int int2 = 2;
    Slice s1 = { sizeof(int), &int1 };
    Slice s2 = { sizeof(int), &int2 };

    res = LINEAR_PUSH(&al, s1);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to push first value");
        deinit_array_list(&al);
        return result;
    }

    res = LINEAR_PUSH(&al, s2);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to push second value");
        deinit_array_list(&al);
        return result;
    }

    res = LINEAR_POP(&al);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to pop second value");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(res.data, s2) != 0) {
        MSG_PRINT(result, " Popped the wrong value for value 2");
        deinit_array_list(&al);
        return result;
    }

    res = LINEAR_POP(&al);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to pop first value");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(res.data, s1) != 0) {
        MSG_PRINT(result, " Popped the wrong value for value 1");
        deinit_array_list(&al);
        return result;
    }

    if (al.item_count != 0) {
        MSG_PRINT(result, " Item count should be 0 after all items popped");
        deinit_array_list(&al);
        return result;
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *array_list_get(TestResult *result) {
    INIT_RESULT(result, "[array_list_get]");

    Allocator *heap = get_raw_heap_allocator();
    Result res = new_array_list(heap, sizeof(int), 16);
    ArrayList al = RESULT_UNWRAP(res, ArrayList);
    int int1 = 1;
    int int2 = 2;
    Slice s1 = { sizeof(int), &int1 };
    Slice s2 = { sizeof(int), &int2 };

    LINEAR_PUSH(&al, s1);
    LINEAR_PUSH(&al, s2);

    res = INDEXING_GET(&al, 0);
    Slice temp = res.data;

    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to get from index 0");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(temp, s1) != 0) {
        MSG_PRINT(result, " Index 0 returned the wrong value");
        deinit_array_list(&al);
        return result;
    }

    res = INDEXING_GET(&al, 1);
    temp = res.data;

    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to get from index 1");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(temp, s2) != 0) {
        MSG_PRINT(result, " Index 1 returned the wrong value");
        deinit_array_list(&al);
        return result;
    }

    res = INDEXING_GET(&al, 2);
    if (res.status == ERROR_OK) {
        MSG_PRINT(result, " Get from out-of-bounds returns ERROR_OK");
        deinit_array_list(&al);
        return result;
    }

    deinit_array_list(&al);
    result->status = TEST_PASS;
    return result;
}

TestResult *array_list_index_of(TestResult *result) {
    INIT_RESULT(result, "[array_list_index_of]");

    Allocator *heap = get_raw_heap_allocator();
    Result res = new_array_list(heap, sizeof(int), 16);
    ArrayList al = RESULT_UNWRAP(res, ArrayList);

    int int1 = 1;
    int int2 = 2;
    int int3 = 3;
    Slice s1 = { sizeof(int), &int1 };
    Slice s2 = { sizeof(int), &int2 };
    Slice s3 = { sizeof(int), &int3 };

    LINEAR_PUSH(&al, s1);
    LINEAR_PUSH(&al, s2);

    res = INDEXING_INDEX_OF(&al, s1);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to get index of s1");
        deinit_array_list(&al);
        return result;
    }
    if (RESULT_UNWRAP(res, unsigned int) != 0) {
        MSG_PRINT(result, " index_of returned the wrong index for s1");
        deinit_array_list(&al);
        return result;
    }

    res = INDEXING_INDEX_OF(&al, s2);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to get index of s2");
        deinit_array_list(&al);
        return result;
    }
    if (RESULT_UNWRAP(res, unsigned int) != 1) {
        MSG_PRINT(result, " index_of returned the wrong index for s2");
        deinit_array_list(&al);
        return result;
    }

    res = INDEXING_INDEX_OF(&al, s3);
    if (res.status == ERROR_OK) {
        MSG_PRINT(result, " index_of did not return an error when given a value not in the collection");
        deinit_array_list(&al);
        return result;
    }

    deinit_array_list(&al);
    result->status = TEST_PASS;

    return result;
}

TestResult *array_list_remove(TestResult *result) {
    INIT_RESULT(result, "[array_list_remove]");

    Allocator *heap = get_raw_heap_allocator();
    ArrayList al = RESULT_UNWRAP(new_array_list(heap, sizeof(int), 16), ArrayList);

    Result res = INDEXING_REMOVE(&al, 0);
    if (res.status == ERROR_OK) {
        MSG_PRINT(result, " remove returned a valid result when given an invalid index");
        deinit_array_list(&al);
        return result;
    }

    int int1 = 1;
    Slice s1 = { sizeof(int), &int1 };
    LINEAR_PUSH(&al, s1);
    res = INDEXING_REMOVE(&al, 0);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " remove return an error when given a valid index");
        deinit_array_list(&al);
        return result;
    }

    if (al.item_count != 0) {
        MSG_PRINT(result, " item count should be zero after item removed");
        deinit_array_list(&al);
        return result;
    }

    deinit_array_list(&al);
    result->status = TEST_PASS;
    return result;
}
