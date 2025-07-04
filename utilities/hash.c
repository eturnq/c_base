#include "hash.h"

uint8_t hash8_slice(Slice text) {
	uint8_t hash = 0;

	for (unsigned int index = 0; index < text.length; index++) {
	    uint8_t byte = ((uint8_t*)text.data)[index];
		hash += byte;
		hash *= byte;
	}

	return hash;
}

static unsigned int hashmap_open_length(Map* map) {
    if (map == 0) {
        return 0;
    }

    return ((HashmapOpen*) map)->data.item_count;
}

static unsigned int hashmap_open_hash(Map* map, Slice key) {
    unsigned int hash;
    if (map == 0 || key.length == 0 || key.data == 0) {
        return -1;
    }

    hash = 0;
    for (int index = 0; index < key.length; index++) {
        uint8_t byte = ((uint8_t*)key.data)[index];
        hash += (unsigned int)byte;
        hash *= (unsigned int)byte;
    }

    return hash % ((HashmapOpen*) map)->offsets.item_count;
}

static Result hashmap_open_add(Map* map, Slice key, Slice value) {
    Result res;
    BASE_ERROR_RESULT(res);

    // TODO: finish this function
    return res;
}

static Result hashmap_open_get(Map* map, Slice key) {
    Result res;
    BASE_ERROR_RESULT(res);

    // TODO: finish this function
    return res;
}

static Result hashmap_open_remove(Map* map, Slice key) {
    Result res;
    BASE_ERROR_RESULT(res);

    // TODO: finish this function
    return res;
}

Result new_hashmap_open(HashmapOpen *hm, Allocator* allocator, unsigned int max_size) {
    Result res;
    BASE_ERROR_RESULT(res);

    if (hm == 0) {
        return res;
    }

    res = new_array_list(&hm->data, allocator, sizeof(struct keyval_pair_s), max_size > 8 ? 8 : max_size);
    if (res.status != ERROR_OK) {
        return res;
    }

    res = new_array_list(&hm->offsets, allocator, sizeof(unsigned int), max_size);
    if (res.status != ERROR_OK) {
        deinit_array_list(&hm->data);
        return res;
    }
    hm->offsets.buffer.length = max_size * sizeof(unsigned int);
    for (unsigned int index = 0; index < max_size; index++) {
        ((int*)hm->offsets.buffer.data)[index] = max_size;
    }

    res = new_stack_collection(allocator, sizeof(unsigned int), max_size > 8 ? 8 : max_size);
    Slice rem_s = res.data;
    if (res.status != ERROR_OK) {
        deinit_array_list(&hm->data);
        deinit_array_list(&hm->offsets);
        return res;
    }
    hm->removals = *((StackCollection*)rem_s.data);

    hm->outside_functions.hash = hashmap_open_hash;
    hm->outside_functions.length = hashmap_open_length;
    hm->outside_functions.add = hashmap_open_add;
    hm->outside_functions.get = hashmap_open_get;
    hm->outside_functions.remove = hashmap_open_remove;

    res.status = ERROR_OK;
    return res;
}
Result deinit_hashmap_open(HashmapOpen *hm) {
    Result res;
    BASE_ERROR_RESULT(res);

    res = deinit_array_list(&hm->data);
    if (res.status != ERROR_OK) return res;
    res = deinit_array_list(&hm->offsets);
    if (res.status != ERROR_OK) return res;
    res = deinit_stack_collection(&hm->removals);
    if (res.status != ERROR_OK) return res;

    res.status = ERROR_OK;
    return res;
}

/*static Slice hashmap8_hash(Slice text) {
    uint8_t hash = hash8_slice(text);
    Slice s = { sizeof(uint8_t), &hash };
    return s;
}

static Result hashmap8_add(Map* map, Slice key, Slice value) {
	Result retval;
	struct keyval_pair_s kvp = { key, value };
	Slice s = { sizeof(struct keyval_pair_s), &kvp };
	BASE_ERROR_RESULT(retval);

	if (map == 0 || IS_NULL_SLICE(key) || IS_NULL_SLICE(value)) {
		return retval;
	}

	Hashmap8 *hashmap = (Hashmap8 *) map;

	uint8_t orig_hash = *((uint8_t*)hashmap->hash(key).data);
	uint8_t hash = orig_hash;
	unsigned int offset;
	do {
	    offset = hashmap->offsets[hash++];
	} while(offset <= MAX_OFFSET && hash != orig_hash);

	if (hash == orig_hash) {
	    return retval;
	}

	retval = LINEAR_POP(&hashmap->removals);
	if (retval.status == ERROR_OK) {
	    offset = RESULT_UNWRAP(retval, unsigned int);
		retval = INDEXING_REPLACE(&hashmap->data, s, offset);
	} else {
	    offset = hashmap->data.item_count;
		retval = LINEAR_PUSH(&hashmap->data, s);
	}

	if (retval.status != ERROR_OK) {
	    retval.data.length = 0;
		retval.data.data = 0;
		return retval;
	}

	hashmap->offsets[--hash] = offset;
	retval.data = value;

	return retval;
}

static Result hashmap8_get(Map* map, Slice key) {
    Result res;
    unsigned int offset;
    Hashmap8 *hm;
    BASE_ERROR_RESULT(res);

    if (map == 0 || key.length == 0 || key.data == 0) {
        return res;
    }

    hm = (Hashmap8*) map;
    if (hm->data.item_count == 0) {
        return res;
    }

    uint8_t orig_hash = *((uint8_t*)hm->hash(key).data);
    uint8_t hash = orig_hash;
    do {
        offset = hm->offsets[hash++];
    } while (offset > MAX_OFFSET && hash != orig_hash);

    if (hash == orig_hash) {
        return res;
    }

    struct keyval_pair_s check_kvp;
    do {
        res = INDEXING_GET(&hm->data, offset);
        Slice check_kvp_s = res.data;
        if (res.status != ERROR_OK) {
            return res;
        }
        check_kvp = *((struct keyval_pair_s*)check_kvp_s.data);
        offset = hm->offsets[hash++];
    } while(offset <= MAX_OFFSET && slice_cmp(check_kvp.key, key) != 0 && hash != orig_hash);

    if (hash == orig_hash) {
        res.data.length = 0;
        res.data.data = 0;
        res.status = ERROR_ERR;
        return res;
    }

    res.data = check_kvp.value;
    return res;
}

static Result hashmap8_remove(Map* map, Slice key) {
    Result res;
    Hashmap8 *hm;
    uint8_t hash, orig_hash;
    unsigned int offset;
    BASE_ERROR_RESULT(res);

    if (map == 0 || key.length == 0 || key.data == 0) {
        return res;
    }

    hm = (Hashmap8*) map;
    if (hm->data.item_count < 1) {
        return res;
    }

    orig_hash = *((uint8_t*)hm->hash(key).data);
    hash = orig_hash;
    do {
        offset = hm->offsets[hash++];
    } while(offset > MAX_OFFSET && hash != orig_hash);

    if (hash == orig_hash) {
        return res;
    }

    do {
        struct keyval_pair_s kvp;
        Slice kvp_s;
        res = INDEXING_GET(&hm->data, offset);
        kvp_s = res.data;
        if (res.status != ERROR_OK) {
            res.data.length = 0;
            res.data.data = 0;
            return res;
        }
        kvp = *((struct keyval_pair_s *)kvp_s.data);

        if (slice_cmp(kvp.key, key) == 0) {
            hm->offsets[offset] = ITEM_REMOVED;

            Slice s = { sizeof(unsigned int), &offset };
            LINEAR_PUSH(&hm->removals, s);

            res.status = ERROR_OK;
            res.data = kvp.value;
            return res;
        }
        offset = hm->offsets[hash++];
    } while (hash != orig_hash);

    res.data.length = 0;
    res.data.data = 0;
    return res;
}

Result new_hashmap8(Allocator* allocator) {
    Result res;
    Hashmap8 hm;
    BASE_ERROR_RESULT(res);

    res = new_array_list(allocator, sizeof(struct keyval_pair_s), MAX_OFFSET + 1);
    Slice data_s = res.data;
    if (res.status != ERROR_OK) {
        return res;
    }
    hm.data = *((ArrayList*)data_s.data);

    for (int index = 0; index <= MAX_OFFSET; index++) {
        hm.offsets[index] = SLOT_EMPTY;
    }

    res = new_stack_collection(allocator, sizeof(unsigned int), MAX_OFFSET + 1);
    Slice rem_s = res.data;
    if (res.status != ERROR_OK) {
        deinit_array_list(&hm.data);
        return res;
    }
    hm.removals = *((StackCollection*)rem_s.data);

    hm.hash = hashmap8_hash;
    hm.outside_functions.add = hashmap8_add;
    hm.outside_functions.get = hashmap8_get;
    hm.outside_functions.remove = hashmap8_remove;

    res.status = ERROR_OK;
    res.data.length = sizeof(Hashmap8);
    res.data.data = &hm;

    return res;
}

Result deinit_hashmap8(Hashmap8* hm) {
    Result res;
    BASE_ERROR_RESULT(res);

    if (hm == 0) {
        return res;
    }

    deinit_array_list(&hm->data);
    deinit_stack_collection(&hm->removals);

    res.status = ERROR_OK;
    return res;
    }*/
