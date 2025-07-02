#include "arraylist.h"

#include <string.h>

static Result array_list_get(Indexing* indexing, int index) {
	Result res;
	ArrayList *al;
	unsigned int offset;
	Slice val;
	BASE_ERROR_RESULT(res);

	if (indexing == 0) {
		return res;
	}

	al = (ArrayList*) indexing;
	if (index < 0) {
		offset = al->item_count + index;
	} else {
		offset = index;
	}

	if (offset >= al->item_count) {
		return res;
	}

	val.length = al->item_size;
	val.data = ((uint8_t*)al->buffer.data + (offset * al->item_size));

	res.status = ERROR_OK;
	res.data = val;

	return res;
}

static Result array_list_index_of(Indexing* indexing, Slice item) {
	Result res;
	ArrayList *al;
	unsigned int item_index;
	Slice val;
	BASE_ERROR_RESULT(res);

	if (indexing == 0 || item.data == 0) {
		return res;
	}

	al = (ArrayList*) indexing;
	if (al->item_size != item.length) {
		return res;
	}

	for (item_index = 0; item_index < al->item_count; item_index++) {
		Slice check_item;
		check_item.length = al->item_size;
		check_item.data = (void*)((uint8_t*)al->buffer.data + (item_index * al->item_size));

		if (slice_cmp(check_item, item) == 0) {
			res.status = ERROR_OK;
			val.length = sizeof(unsigned int);
			val.data = &item_index;
			res.data = val;
			return res;
		}
	}

	return res;
}

static Result array_list_remove(Indexing *indexing, int index) {
	Result res;
	ArrayList *al;
	unsigned int offset;
	BASE_ERROR_RESULT(res);

	if (indexing == 0) {
		return res;
	}

	al = (ArrayList*) indexing;

	if (index < 0) {
		offset = al->item_count + index;
	} else {
		offset = index;
	}

	if (offset >= al->item_count) {
		return res;
	}

	if (offset < al->item_count - 1) {
		memcpy(al->buffer.data, (uint8_t*)al->buffer.data + (offset * al->item_size), al->buffer.length - (offset * al->item_size));
	}
	al->item_count--;

	res.status = ERROR_OK;
	return res;
}

static Result array_list_insert(Indexing* indexing, Slice item, int index) {
	Result res, alloc_res;
	ArrayList *al;
	unsigned int offset;
	BASE_ERROR_RESULT(res);

	if (indexing == 0 || item.data == 0) {
		return res;
	}

	al = (ArrayList*) indexing;
	if (item.length != al->item_size) {
		return res;
	}

	if (al->item_count * al->item_size >= al->buffer.length) {
		alloc_res = REALLOC(al->allocator, al->buffer, al->buffer.length << 1);
		if (alloc_res.status != ERROR_OK) {
			return res;
		}

		al->buffer = alloc_res.data;
	}

	if (index < 0) {
		offset = al->item_count + index;
	} else {
		offset = al->item_count - index;
	}

	if (offset >= al->item_count) {
		return res;
	}

	alloc_res = ALLOC(al->allocator, (al->item_count - offset) * al->item_size);
	if (alloc_res.status != ERROR_OK) {
		return res;
	}

	memcpy(alloc_res.data.data, ((uint8_t*)al->buffer.data) + (offset * al->item_size), alloc_res.data.length);
	memcpy(((uint8_t*)al->buffer.data) + (offset * al->item_size), item.data, item.length);
	memcpy(((uint8_t*)al->buffer.data) + ((offset + 1) * al->item_size), alloc_res.data.data, alloc_res.data.length);
	FREE(al->allocator, alloc_res.data);
	al->item_count++;

	res.status = ERROR_OK;
	return res;
}

static Result array_list_swap(Indexing *indexing, int index_a, int index_b) {
	Result res;
	ArrayList *al;
	Slice a, b, temp;
	unsigned int a_offset, b_offset;

	BASE_ERROR_RESULT(res);

	if (indexing == 0) {
		return res;
	}

	al = (ArrayList*) indexing;

	if (index_a < 0) {
		a_offset = al->item_count + index_a;
	} else {
		a_offset = index_a;
	}

	if (a_offset >= al->item_count) {
		return res;
	}

	if (index_b < 0) {
		b_offset = al->item_count + index_a;
	} else {
		b_offset = index_a;
	}

	if (b_offset >= al->item_count) {
		return res;
	}

	a_offset *= al->item_size;
	b_offset *= al->item_size;
	a = slice_sub(al->buffer, a_offset, al->item_size);
	b = slice_sub(al->buffer, b_offset, al->item_size);

	res = CLONE(al->allocator, a);
	temp = res.data;
	if (res.status != ERROR_OK) {
	    return res;
	}

	res = slice_copy(a, b);
	if (res.status != ERROR_OK) {
	    return res;
	}

	res = slice_copy(b, temp);
	if (res.status != ERROR_OK) {
	    return res;
	}

	res = FREE(al->allocator, temp);
	if (res.status != ERROR_OK) {
	    return res;
	}

	res.status = ERROR_OK;
	res.data.length = 0;
	res.data.data = 0;

	return res;
}

static Result array_list_replace(Indexing *indexing, Slice item, int index) {
    Result res;
    ArrayList *al;
    Slice dest;
    unsigned int offset;
    BASE_ERROR_RESULT(res);

    if (indexing == 0 || item.data == 0) {
        return res;
    }

    al = (ArrayList *) indexing;
    if (al->item_size != item.length) {
        return res;
    }

    if (index < 0) {
        offset = al->item_count + index;
    } else {
        offset = index;
    }

    if (offset >= al->item_count) {
        return res;
    }
    offset *= al->item_size;

    dest = slice_sub(al->buffer, offset, al->item_size);
    res = slice_copy(dest, item);

    return res;
}

static Result array_list_push(Linear *linear, Slice item) {
    Result res;
    ArrayList *al;
    unsigned int offset;
    BASE_ERROR_RESULT(res);

    if (linear == 0 || item.data == 0) {
        return res;
    }

    al = (ArrayList *) linear;
    if (item.length != al->item_size) {
        return res;
    }

    offset = al->item_count * al->item_size;
    if (offset >= al->buffer.length) {
        res = REALLOC(al->allocator, al->buffer, al->buffer.length << 1);
        if (res.status != ERROR_OK) {
            return res;
        }
        al->buffer = res.data;
    }

    res = slice_copy(slice_sub(al->buffer, offset, al->item_size), item);
    if (res.status != ERROR_OK) {
        return res;
    }

    al->item_count++;

    return res;
}

static Result array_list_pop(Linear *linear) {
    Result res;
    ArrayList *al;
    BASE_ERROR_RESULT(res);

    if (linear == 0) {
        return res;
    }

    al = (ArrayList *) linear;
    if (al->item_count < 1) {
        return res;
    }

    al->item_count--;
    res.data = slice_sub(al->buffer, (al->item_count) * al->item_size, al->item_size);
    res.status = ERROR_OK;

    return res;
}

Result new_array_list(Allocator* allocator, unsigned int item_size, unsigned int max_count) {
	Result res, alloc_res;
	ArrayList al;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || item_size == 0 || max_count == 0) {
		return res;
	}

	al.allocator = allocator;
	al.item_size = item_size;
	al.item_count = 0;
	alloc_res = ALLOC(allocator, item_size * max_count);
	if (alloc_res.status != ERROR_OK) {
		return res;
	}
	al.buffer = alloc_res.data;
	memset(al.buffer.data, 0, al.buffer.length);

	al.outside_functions.get = array_list_get;
	al.outside_functions.index_of = array_list_index_of;
	al.outside_functions.remove = array_list_remove;
	al.outside_functions.insert = array_list_insert;
	al.outside_functions.swap = array_list_swap;
	al.outside_functions.replace = array_list_replace;
	al.outside_functions.linear_functions.push = array_list_push;
	al.outside_functions.linear_functions.pop = array_list_pop;

	res.status = ERROR_OK;
	res.data.length = sizeof(ArrayList);
	res.data.data = &al;
	return res;
}

Result deinit_array_list(ArrayList* al) {
	Result res, free_res;
	BASE_ERROR_RESULT(res);

	free_res = FREE(al->allocator, al->buffer);
	if (free_res.status != ERROR_OK) {
		return res;
	}

	al->item_count = 0;
	al->item_size = 0;

	res.status = ERROR_OK;
	return res;
}
