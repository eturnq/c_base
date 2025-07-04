#include "arraylist_test.h"
#include "../utilities.h"

TestResult *array_list_init_deinit(TestResult *result) {
    ArrayList al;
    INIT_RESULT(result, "[array_list_init_deinit]");

    Allocator *heap = get_raw_heap_allocator();
    Result res = new_array_list(&al, heap, sizeof(int), 16);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to create new ArrayList");
        return result;
    }

    res = deinit_array_list(&al);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to deinit ArrayList");
        return result;
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *array_list_push(TestResult *result) {
    ArrayList al;
    INIT_RESULT(result, "[array_list_push]");

    Allocator *heap = get_raw_heap_allocator();
    new_array_list(&al, heap, sizeof(int), 16);

    int int1 = 1;
    int int2 = 2;
    int int3 = 3;
    Slice s = { sizeof(int), &int1 };

    if (LINEAR_PUSH(&al, s).status != ERROR_OK) {
        MSG_PRINT(result, " Unable to push value 1");
        deinit_array_list(&al);
        return result;
    }
    if (al.item_count != 1) {
        MSG_PRINT(result, " Incorrect item_count after 1 push");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(s, slice_sub(al.buffer, 0, al.item_size)) != 0) {
        MSG_PRINT(result, " Incorrect value of index 0 after 1 push");
        deinit_array_list(&al);
    }

    s.data = &int2;
    if (LINEAR_PUSH(&al, s).status != ERROR_OK) {
        MSG_PRINT(result, " Unable to push value 2");
        deinit_array_list(&al);
        return result;
    }
    if (al.item_count != 2) {
        MSG_PRINT(result, " Incorrect item_count after 2 pushes");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(s, slice_sub(al.buffer, al.item_size, al.item_size)) != 0) {
        MSG_PRINT(result, " Incorrect value of index 0 after 2 pushes");
        deinit_array_list(&al);
    }

    s.data = &int3;
    if (LINEAR_PUSH(&al, s).status != ERROR_OK) {
        MSG_PRINT(result, " Unable to push value 3");
        deinit_array_list(&al);
        return result;
    }
    if (al.item_count != 3) {
        MSG_PRINT(result, " Incorrect item_count after 3 pushes");
        deinit_array_list(&al);
        return result;
    }
    if (slice_cmp(s, slice_sub(al.buffer, al.item_size << 1, al.item_size)) != 0) {
        MSG_PRINT(result, " Incorrect value of index 0 after 3 pushes");
        deinit_array_list(&al);
    }

    deinit_array_list(&al);
    result->status = TEST_PASS;
    return result;
}

TestResult *array_list_pop(TestResult *result) {
    ArrayList al;
    INIT_RESULT(result, "[array_list_push_pop]");

    Allocator *heap = get_raw_heap_allocator();
    Result res = new_array_list(&al, heap, sizeof(int), 16);
    int int1 = 1;
    int int2 = 2;
    Slice s1 = { sizeof(int), &int1 };
    Slice s2 = { sizeof(int), &int2 };

    res = LINEAR_PUSH(&al, s1);
    res = LINEAR_PUSH(&al, s2);

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
    ArrayList al;
    Result res;
    INIT_RESULT(result, "[array_list_get]");

    Allocator *heap = get_raw_heap_allocator();
    new_array_list(&al, heap, sizeof(int), 16);
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
    ArrayList al;
    Result res;
    INIT_RESULT(result, "[array_list_index_of]");

    Allocator *heap = get_raw_heap_allocator();
    new_array_list(&al, heap, sizeof(int), 16);

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
    ArrayList al;
    Result res;
    INIT_RESULT(result, "[array_list_remove]");

    Allocator *heap = get_raw_heap_allocator();
    new_array_list(&al, heap, sizeof(int), 16);

    res = INDEXING_REMOVE(&al, 0);
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

TestResult *array_list_insert(TestResult *result) {
    ArrayList al;
    INIT_RESULT(result, "[array_list_insert]");

    Allocator *heap = get_raw_heap_allocator();
    new_array_list(&al, heap, sizeof(int), 16);
    int int1 = 1;
    int int2 = 2;
    int int3 = 3;
    Slice s = { sizeof(int), &int1 };
    LINEAR_PUSH(&al, s);
    s.data = &int2;
    LINEAR_PUSH(&al, s);
    s.data = &int3;

    Result res = INDEXING_INSERT(&al, s, 1);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to insert value");
        deinit_array_list(&al);
        return result;
    }

    if (al.item_count != 3) {
        MSG_PRINT(result, " Incorrect item_count after insert");
        deinit_array_list(&al);
        return result;
    }

    if (RESULT_UNWRAP(INDEXING_GET(&al, 0), int) != int1) {
        MSG_PRINT(result, " Index 0 has the wrong value");
        deinit_array_list(&al);
        return result;
    }

    if (RESULT_UNWRAP(INDEXING_GET(&al, 1), int) != int3) {
        MSG_PRINT(result, " Index 1 has the wrong value");
        deinit_array_list(&al);
        return result;
    }

    if (RESULT_UNWRAP(INDEXING_GET(&al, 2), int) != int2) {
        MSG_PRINT(result, " Index 2 has the wrong value");
        deinit_array_list(&al);
        return result;
    }

    deinit_array_list(&al);
    result->status = TEST_PASS;

    return result;
}

TestResult *array_list_swap(TestResult *result) {
    ArrayList al;
    INIT_RESULT(result, "[array_list_swap]");

    Allocator *heap = get_raw_heap_allocator();
    new_array_list(&al, heap, sizeof(int), 16);

    int int1 = 1;
    int int2 = 2;
    int int3 = 3;
    Slice s = { sizeof(int), &int1 };
    LINEAR_PUSH(&al, s);
    s.data = &int2;
    LINEAR_PUSH(&al, s);
    s.data = &int3;
    LINEAR_PUSH(&al, s);

    if (INDEXING_SWAP(&al, 0, 2).status != ERROR_OK) {
        MSG_PRINT(result, " Unable to perform swap");
        deinit_array_list(&al);
        return result;
    }

    if (al.item_count != 3) {
        MSG_PRINT(result, " Incorrect item_count after swap");
        deinit_array_list(&al);
        return result;
    }

    if (RESULT_UNWRAP(INDEXING_GET(&al, 0), int) != 3) {
        MSG_PRINT(result, " Index 0 has incorrect value");
        deinit_array_list(&al);
        return result;
    }

    if (RESULT_UNWRAP(INDEXING_GET(&al, 1), int) != 2) {
        MSG_PRINT(result, " Index 1 has incorrect value");
        deinit_array_list(&al);
        return result;
    }

    if (RESULT_UNWRAP(INDEXING_GET(&al, 2), int) != 1) {
        MSG_PRINT(result, " Index 2 has incorrect value");
        deinit_array_list(&al);
        return result;
    }

    deinit_array_list(&al);
    result->status = TEST_PASS;
    return result;
}

TestResult *array_list_replace(TestResult *result) {
    ArrayList al;
    INIT_RESULT(result, "[array_list_replace]");

    Allocator *heap = get_raw_heap_allocator();
    new_array_list(&al, heap, sizeof(int), 16);

    int int1 = 1;
    int int2 = 2;
    int int3 = 3;
    Slice s = { sizeof(int), &int1 };

    LINEAR_PUSH(&al, s);
    s.data = &int2;
    LINEAR_PUSH(&al, s);
    s.data = &int3;

    if (INDEXING_REPLACE(&al, s, 0).status != ERROR_OK) {
        MSG_PRINT(result, " Unable to do first replacement");
        deinit_array_list(&al);
        return result;
    }

		if (al.item_count != 2) {
			MSG_PRINT(result, " Item count incorrect after 1 replace");
			deinit_array_list(&al);
			return result;
		}

		if (RESULT_UNWRAP(INDEXING_GET(&al, 0), int) != 3) {
			MSG_PRINT(result, " Index 0 has incorrect value after 1 replace");
			deinit_array_list(&al);
			return result;
		}

		if (RESULT_UNWRAP(INDEXING_GET(&al, 1), int) != 2) {
			MSG_PRINT(result, " Index 1 has incorrect value after 1 replace");
			deinit_array_list(&al);
			return result;
		}

		if (INDEXING_REPLACE(&al, s, 1).status != ERROR_OK) {
			MSG_PRINT(result, " Unable to do second replacement");
			deinit_array_list(&al);
			return result;
		}

		if (al.item_count != 2) {
			MSG_PRINT(result, " Item count incorrect after 2 replaces");
			deinit_array_list(&al);
			return result;
		}

		if (RESULT_UNWRAP(INDEXING_GET(&al, 0), int) != 3) {
			MSG_PRINT(result, " Index 0 has incorrect value after 2 replaces");
			deinit_array_list(&al);
			return result;
		}

		if (RESULT_UNWRAP(INDEXING_GET(&al, 1), int) != 3) {
			MSG_PRINT(result, " Index 1 has incorrect value after 2 replaces");
			deinit_array_list(&al);
			return result;
		}

		result->status = TEST_PASS;
    return result;
}
