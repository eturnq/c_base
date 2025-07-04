#include "hash_test.h"
//#include "../utilities.h"

TestResult *hashmap_open_init_deinit(TestResult *result) {
    Allocator *heap;
    Slice hm_s;
    Result res;
    INIT_RESULT(result, "[hashmap_open_init_deinit]");

    heap = get_raw_heap_allocator();
    res = new_hashmap_open(heap, 128);
    hm_s = res.data;
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to initialize");
        return result;
    }

    res = deinit_hashmap_open(hm_s.data);
    if (res.status != ERROR_OK) {
        MSG_PRINT(result, " Unable to deinitialize. Possible memory leak!");
        return result;
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *hashmap_open_add_get(TestResult *result) {
    Allocator *heap;
    HashmapOpen hm;
    Result res;
    Slice keys[16];
    Slice values[16];
    int ints[16];
    Slice key_bytes;
    INIT_RESULT(result, "[hashmap_open_add_get]");

    heap = get_raw_heap_allocator();
    hm = RESULT_UNWRAP(new_hashmap_open(heap, 32), HashmapOpen);

    key_bytes.data = "int0int1int2int3int4int5int6int7int7int9intAintBintCintDintEintF";
    key_bytes.length = sizeof(unsigned char) * 16 * 4;
    for (int index = 0; index < 16; index++) {
        ints[index] = index;
        values[index].length = sizeof(int);
        values[index].data = &ints[index];
        keys[index] = slice_sub(key_bytes, index << 2, 4);

        res = MAP_ADD(&hm, keys[index], values[index]);
        Slice res_data = res.data;
        if (res.status != ERROR_OK) {
            deinit_hashmap_open(&hm);
            sprintf(result->message + strlen(result->message), " Unable to perform add %d", index);
            return result;
        }
        if (slice_cmp(*(Slice*)res_data.data, values[index]) != 0) {
            deinit_hashmap_open(&hm);
            sprintf(result->message + strlen(result->message), " Add %d returned incorrect value", index);
            return result;
        }

        for (int index2 = index; index2 <= 0; index2++) {
            res = MAP_GET(&hm, keys[index2]);
            res_data = res.data;
            if (res.status != ERROR_OK) {
                deinit_hashmap_open(&hm);
                sprintf(result->message + strlen(result->message), " Unable to perform get %d after add %d", index2, index);
                return result;
            }
            if (slice_cmp(*(Slice*)res_data.data, values[index]) != 0) {
                deinit_hashmap_open(&hm);
                sprintf(result->message + strlen(result->message), " Get %d returned incorrect value after add %d", index2, index);
            }
        }
    }

    deinit_hashmap_open(&hm);
    return result;
}

/*TestResult *hashmap8_init_deinit(TestResult *result) {
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
    Slice keys[3];
    Slice values[3];
    int ints[3];
    struct keyval_pair_s kvp;
    Result res;
    INIT_RESULT(result, "[hashmap8_add]");

    heap = get_raw_heap_allocator();
    hm = RESULT_UNWRAP(new_hashmap8(heap), Hashmap8);

    keys[0].data = "int0";
    keys[0].length = strlen(keys[0].data);
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

    for (int index = 0; index < (int)hm.data.item_count; index++) {
        kvp = ((struct keyval_pair_s*)hm.data.buffer.data)[index];
        if (slice_cmp(kvp.key, keys[index]) != 0) {
            deinit_hashmap8(&hm);
            sprintf(result->message, " Invalid key at index %d after 2 adds", index);
            return result;
        }
        if (slice_cmp(kvp.value, values[index]) != 0) {
            deinit_hashmap8(&hm);
            sprintf(result->message, " Invalid value at index %d after 2 adds", index);
            return result;
        }
    }

    res = MAP_ADD(&hm, keys[2], values[2]);
    if (res.status != ERROR_OK) {
        deinit_hashmap8(&hm);
        MSG_PRINT(result, " Unable to perform third add");
        return result;
    }

    if (hm.data.item_count != 3) {
        deinit_hashmap8(&hm);
        MSG_PRINT(result, " Invalid item count after 3 adds");
        return result;
    }

    for (int index = 0; index < (int)hm.data.item_count; index++) {
        kvp = ((struct keyval_pair_s *)hm.data.buffer.data)[index];
        if (slice_cmp(kvp.key, keys[index]) != 0) {
            deinit_hashmap8(&hm);
            sprintf(result->message, " Invalid key at index %d after 3 adds", index);
            return result;
        }
        if (slice_cmp(kvp.value, values[index]) != 0) {
            deinit_hashmap8(&hm);
            sprintf(result->message, " Invalid value at index %d after 3 adds", index);
            return result;
        }
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *hashmap8_get(TestResult *result) {
    Allocator *heap;
    Hashmap8 hm;
    Slice keys[3];
    Slice values[3];
    int ints[3];
    Result res;
    INIT_RESULT(result, "[hashmap8_get]");

    for (int index = 0; index < 3; index++) {
        ints[index] = index;
        values[index].data = &ints[index];
        values[index].length = sizeof(int);
    }
    keys[0].data = "int0";
    keys[0].length = strlen(keys[0].data);
    keys[1].data = "int1";
    keys[1].length = keys[0].length;
    keys[2].data = "int2";
    keys[2].length = keys[0].length;

    heap = get_raw_heap_allocator();
    hm = RESULT_UNWRAP(new_hashmap8(heap), Hashmap8);

    for (int index = 0; index < 3; index++) {
        MAP_ADD(&hm, keys[index], values[index]);
    }

    for (int index = 0; index < 3; index++) {
        res = MAP_GET(&hm, keys[index]);
        Slice kvp_s = res.data;
        if (res.status != ERROR_OK) {
            deinit_hashmap8(&hm);
            sprintf(result->message + strlen(result->message), " Unable to perform get with key %d", index);
            return result;
        }
        struct keyval_pair_s kvp = *(struct keyval_pair_s *)kvp_s.data;
        if (kvp.key.data == 0 || kvp.key.length != keys[index].length) {
            deinit_hashmap8(&hm);
            sprintf(result->message + strlen(result->message), " Invalid key data after get %d", index);
            return result;
        }
        if (kvp.value.data == 0 || kvp.value.length != values[index].length) {
            deinit_hashmap8(&hm);
            sprintf(result->message + strlen(result->message), " Invalid value data after get %d", index);
            return result;
        }

        if (hm.data.item_count != 3) {
            deinit_hashmap8(&hm);
            sprintf(result->message + strlen(result->message), " Invalid item count after get %d", index);
            return result;
        }

        if (slice_cmp(kvp.key, keys[index]) != 0) {
            deinit_hashmap8(&hm);
            sprintf(result->message + strlen(result->message), " Invalid key after get %d", index);
            return result;
        }
        if (slice_cmp(kvp.value, values[index]) != 0) {
            deinit_hashmap8(&hm);
            sprintf(result->message + strlen(result->message), " Invalid value after ger %d", index);
            return result;
        }
    }

    result->status = TEST_PASS;
    return result;
}

TestResult *hashmap8_remove(TestResult *result) {
    Allocator *heap;
    Hashmap8 hm;
    Result res;
    Slice keys[3];
    Slice values[3];
    int ints[3];
    INIT_RESULT(result, "[hashmap8_remove]");

    for (int index = 0; index < 3; index++) {
        ints[index] = index;
        values[index].length = sizeof(int);
        values[index].data = &ints[index];
    }
    keys[0].data = "int0";
    keys[0].length = strlen(keys[0].data) - 1;
    keys[1].data = "int1";
    keys[1].length = keys[0].length;
    keys[2].data = "int2";
    keys[2].length = keys[0].length;

    heap = get_raw_heap_allocator();
    hm = RESULT_UNWRAP(new_hashmap8(heap), Hashmap8);

    for (int index = 0; index < 3; index++) {
        MAP_ADD(&hm, keys[index], values[index]);
    }

    res = MAP_REMOVE(&hm, keys[1]);
    if (res.status != ERROR_OK) {
        deinit_hashmap8(&hm);
        MSG_PRINT(result, " Unable to perform first remove");
        return result;
    }
    if (slice_cmp(res.data, values[1]) != 0) {
        deinit_hashmap8(&hm);
        MSG_PRINT(result, " Invalid return value after first remove");
        return result;
    }
    if (hm.removals.item_count != 1) {
        deinit_hashmap8(&hm);
        MSG_PRINT(result, " Invalid item_count of removals after first remove");
        return result;
    }

    // TODO: finish this test
    MSG_PRINT(result, " Unimplemented");
    return result;
    }*/
