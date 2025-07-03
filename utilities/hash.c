#include "hash.h"

uint8_t hash8_slice(Slice text) {
	uint8_t hash = 0;

	for (unsigned int index = 0; index < text.length; index++) {
	    uint8_t byte = ((uint8_t*)text.data)[index];
		hash += byte;
	}

	return hash;
}

struct keyval_pair_s {
    Slice key;
    Slice value;
};

static Slice hashmap8_hash(Hashmap8 *map, Slice text) {
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

	uint8_t orig_hash = *((uint8_t*)hashmap->hash(hashmap, key).data);
	uint8_t hash = orig_hash;
	unsigned int offset;
	do {
	    offset = hashmap->offsets[hash++];
	} while(offset <= MAX_OFFSET && hash != orig_hash);

	if (hash == orig_hash) {
	    return retval;
	}

	hash--; // empty spot found. Revert last increment
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

	hashmap->offsets[hash] = offset;
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

    uint8_t orig_hash = *((uint8_t*)hm->hash(hm, key).data);
    uint8_t hash = orig_hash;
    do {
        offset = hm->offsets[hash++];
    } while (offset > MAX_OFFSET || hash != orig_hash);

    if (hash == orig_hash) {
        return res;
    }

    struct keyval_pair_s check_kvp;
    do {
        res = INDEXING_GET(&hm->data, offset);
        if (res.status != ERROR_OK) {
            return res;
        }
        check_kvp = RESULT_UNWRAP(res, struct keyval_pair_s);
        offset = hm->offsets[hash++];
    } while(slice_cmp(check_kvp.key, key) != 0 && hash != orig_hash);

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

    orig_hash = *((uint8_t*)hm->hash(hm, key).data);
    hash = orig_hash;
    do {
        offset = hm->offsets[hash++];
    } while(offset < MAX_OFFSET || hash != orig_hash);

    if (hash == orig_hash) {
        return res;
    }

    do {
        struct keyval_pair_s kvp;
        res = INDEXING_GET(&hm->data, offset);
        if (res.status != ERROR_OK) {
            res.data.length = 0;
            res.data.data = 0;
            return res;
        }

        kvp = RESULT_UNWRAP(res, struct keyval_pair_s);
        if (slice_cmp(kvp.key, key) == 0) {
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
    BASE_ERROR_RESULT(res);

    // TODO: finish this function

    return res;
}

Result deinit_hashmap8(Hashmap8* hm) {
    Result res;
    BASE_ERROR_RESULT(res);

    // TODO: finish this function

    return res;
}
