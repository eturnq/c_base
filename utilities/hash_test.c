#include "hash_test.h"
#include "hash.h"
#include "../utilities.h"

TestResult *hashmap8_init_deinit(TestResult *result) {
    Result res;
    Allocator *heap;
    INIT_RESULT(result, "[hashmap8_init_deinit]");

    heap = get_raw_heap_allocator();
    res = new_hashmap8(heap);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to instantiate Hashmap8");
        return result;
    }

    if (deinit_hashmap8((Hashmap8*)res.data.data).status != ERROR_OK) {
        MSG_PRINT(result, " Unable to deinit Hashmap8");
        return result;
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *hashmap8_add(TestResult *result) {
    Allocator *heap;
    Hashmap8 hm;
    //Slice key, value;
    Slice keys[3];
    Slice values[3];
    int ints[3];
    struct keyval_pair_s kvp;
    Result res;
    INIT_RESULT(result, "[hashmap8_add]");

    heap = get_raw_heap_allocator();
    hm = RESULT_UNWRAP(new_hashmap8(heap), Hashmap8);

    keys[0].data = "int0";
    keys[0].length = strlen(keys[0].data - 1);
    keys[1].data = "int1";
    keys[1].length = keys[0].length;
    keys[2].data = "int2";
    keys[2].length = keys[0].length;

    for (int index = 0; index < 3; index++) {
        ints[index] = index;
        values[index].length = sizeof(int);
        values[index].data = &ints[index];
    }
    res = MAP_ADD(&hm, keys[0], values[0]);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to perform first add");
        deinit_hashmap8(&hm);
        return result;
    }

    if (hm.data.item_count != 1) {
        deinit_hashmap8(&hm);
        MSG_PRINT(result, " Invalid item count after 1 add");
        return result;
    }

    kvp = ((struct keyval_pair_s*)hm.data.buffer.data)[0];
    if (slice_cmp(kvp.key, keys[0]) != 0) {
        deinit_hashmap8(&hm);
        MSG_PRINT(result, " Invalid key at index 0 after 1 add");
    }
    if (slice_cmp(kvp.value, values[0]) != 0) {
        deinit_hashmap8(&hm);
        MSG_PRINT(result, " Invalid value at index 0 after 1 add");
    }

    res = MAP_ADD(&hm, keys[1], values[1]);
    if (res.status != ERROR_OK) {
        deinit_hashmap8(&hm);
        MSG_PRINT(result, " Unable to perform second add");
        return result;
    }

    if (hm.data.item_count != 2) {
        deinit_hashmap8(&hm);
        MSG_PRINT(result, " Invalid item count after 2 adds");
        return result;
    }

    for (int index = 0; index < hm.data.item_count; index++) {
        kvp = ((struct keyval_pair_s*)hm.data.buffer.data)[index];
        if (slice_cmp(kvp.key, keys[index]) != 0) {
            deinit_hashmap8(&hm);
            sprintf(result->message, " Invalid key at index %d after 2 adds", index);
            return result;
        }
        if (slice_cmp(kvp.value, values[index]) != 0) {
            // TODO:
        }
    }

    // TODO: finish this test
    MSG_PRINT(result, " Unimplemented");
    return result;
}

TestResult *hashmap8_get(TestResult *result) {
    INIT_RESULT(result, "[hashmap8_get]");

    // TODO: finish this test
    MSG_PRINT(result, " Unimplemented");
    return result;
}

TestResult *hashmap8_remove(TestResult *result) {
    INIT_RESULT(result, "[hashmap8_remove]");

    // TODO: finish this test
    MSG_PRINT(result, " Unimplemented");
    return result;
}
