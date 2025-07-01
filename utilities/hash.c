#include "../utilities.h"

uint8_t hash8_slice(Slice text) {
	uint8_t hash = 0;

	for (unsigned int index = 0; index < text.length; index++) {
		hash += ((uint8_t*)text.data)[index];
	}

	return hash;
}

struct hashmap8_s {
	Map outside_functions;
	Allocator *allocator;
	Slice key_data;
	Slice values;
	uint8_t offsets[256];
	unsigned int item_size;
	unsigned int item_count;
};

static Result hashmap8_add(Map* map, Slice key, Slice value) {
	Result retval;
	BASE_ERROR_RESULT(retval);

	if (map == 0 || IS_NULL_SLICE(key) || IS_NULL_SLICE(value)) {
		return retval;
	}

	Hashmap8 *hashmap = (Hashmap8 *) map;
	if (hashmap->item_count >= hashmap->key_data.length) {
		return retval;	
	}

	// TODO: implement this function
	return retval;
	/*uint8_t orig_hash = hash8_slice(key);
	uint8_t key_hash = orig_hash;
	while (hashmap->keys.data[key_hash] != 0) {
		if (++key_hash == orig_hash) {
			return retval;
		}
	}

	((Slice*)hashmap->keys.data)[key_hash] = */
}
