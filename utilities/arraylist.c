#include "arraylist.h"
#include "stack.h"

#include <string.h>

static Result array_list_get(Indexing* indexing, int index) {
	Result res;
	ArrayList *al;
	unsigned int offset;
	static Slice val;
	BASE_ERROR_RESULT(res);

	if (indexing == 0) {
		return res;
	}

	al = (ArrayList*) indexing;
	if (index < 0) {
		offset = al->item_count + index;
	} else {
		offset = al->item_count - index;
	}

	if (offset > al->item_count) {
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
	static unsigned int item_index;
	static Slice val;
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
		offset = al->item_count - index;
	}

	if (offset > al->item_count) {
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

	res.status = ERROR_OK;
	return res;
}

Result new_array_list(Allocator* allocator, unsigned int item_size, unsigned int max_count) {
	Result res, alloc_res;
	static ArrayList al;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || item_size == 0 || max_count == 0) {
		return res;
	}

	/*stack_res = new_stack_collection(allocator, item_size, max_count);
	if (stack_res.status != ERROR_OK) {
		return res;
	}
	StackCollection stack = *((StackCollection*)stack_res.data.data);*/

	al.allocator = allocator;
	al.item_size = item_size;
	al.item_count = 0;
	alloc_res = ALLOC(allocator, item_size * max_count);
	if (alloc_res.status != ERROR_OK) {
		return res;
	}
	al.buffer = alloc_res.data;

	al.outside_functions.get = array_list_get;
	al.outside_functions.index_of = array_list_index_of;
	al.outside_functions.remove = array_list_remove;
	al.outside_functions.insert = array_list_insert;
	// TODO: set outside functions
	
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
