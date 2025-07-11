#include "../globals.h"
#include "../memory.h"
#include "../utilities.h"

#include <stdlib.h>
#include <string.h>

// This function assumes that FREE might fail and destroy data, but not a valid reference
Result standard_realloc(Allocator *allocator, Slice ptr, unsigned int size) {
	Result res;
	Slice new_mem;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.data == 0 || ptr.length == 0 || size == 0) {
		return res;
	}

	res = ALLOC(allocator, size);
	if (res.status != ERROR_OK) {
		return res;
	}
	if (res.data.length != size) {
		FREE(allocator, res.data);
		BASE_ERROR_RESULT(res);
		return res;
	}
	new_mem = res.data;	

	res = slice_copy(new_mem, ptr);
	if (res.status != ERROR_OK) {
		FREE(allocator, new_mem);
		return res;
	}

	res = FREE(allocator, ptr);
	if (res.status != ERROR_OK) {
		slice_copy(ptr, new_mem);
		FREE(allocator, new_mem);
		BASE_ERROR_RESULT(res);
		return res;
	}

	res.status = ERROR_OK;
	res.data = new_mem;
	return res;
}

function Result array_list_push_slice(ArrayList *al, Slice whole, unsigned int index, unsigned int last_offset) {
	Result res;
	#ifdef DEBUG_SET
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
	char *to_add_str;
	#pragma GCC diagnostic pop
    	#endif

	BASE_ERROR_RESULT(res); 

	res = ALLOC(al->allocator, sizeof(Slice));
	if (res.status != ERROR_OK) {
		return res;
	}
	if (res.data.length != sizeof(Slice)) {
		FREE(al->allocator, res.data);
		BASE_ERROR_RESULT(res);
		return res;
	}
	Slice *s = (Slice *) res.data.data;
	s->data = (void*) ((uint8_t *) whole.data) + last_offset;
	s->length = index - last_offset;

	#ifdef DEBUG_SET
	to_add_str = (char *) s->data;
    	#endif

	Slice to_add;
	to_add.data = s;
	to_add.length = sizeof(Slice);

	if (to_add.length > 0) {
		res = LINEAR_PUSH(al, to_add);
		if (res.status != ERROR_OK) {
    			res.data.data = (void*) s;
    			res.data.length = sizeof(Slice);
    			FREE(al->allocator, res.data);
    			SET_NULL_SLICE(res.data); 
			return res;
		}
	}	
	res.status = ERROR_OK;
	return res;
}

Result standard_slice_split(Allocator *allocator, Slice whole, Slice part) {
	Result res;
	ArrayList *parts;
	Slice check;
	#ifdef DEBUG_SET
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wunused-variable"
	char *whole_str = (char*) whole.data;
	char *part_str = (char*) part.data;
	#pragma GCC diagnostic pop
	#endif
	BASE_ERROR_RESULT(res); 

	if (
    		allocator == 0 	  ||
   		whole.length == 0 || whole.data == 0 ||
		part.length == 0  || part.data == 0
	) {
		return res;
	}

	res = ALLOC(allocator, sizeof(ArrayList));
	if (res.status != ERROR_OK) {
		return res;
	}
	if (res.data.length != sizeof(ArrayList)) {
		FREE(allocator, res.data);
		BASE_ERROR_RESULT(res); 
		return res;
	}
	parts = (ArrayList *) res.data.data;
	res = new_array_list(parts, allocator, sizeof(Slice), 128); 

	unsigned int last_part_offset = 0;
	check.length = part.length;
	check.data = whole.data;
	for (unsigned int index = 0; index < whole.length - part.length; index++) {
		check.data = (void*) ((uint8_t *) whole.data) + index;
		#ifdef DEBUG_SET
		#pragma GCC diagnostic push
		#pragma GCC diagnostic ignored "-Wunused-variable"
		char *check_str = (char *) check.data;
		#pragma GCC diagnostic pop
		#endif
		if (slice_cmp(check, part) == 0) {
    			res = array_list_push_slice(parts, whole, index, last_part_offset); 
    			if (res.status != ERROR_OK) {
				res.data.data = parts;
				res.data.length = sizeof(ArrayList);
    				FREE(allocator, res.data);
    				BASE_ERROR_RESULT(res); 				
    				return res;
    			}
    			
			last_part_offset = index + 1;
		}
	}

	res = array_list_push_slice(parts, whole, whole.length, last_part_offset);
	if (res.status != ERROR_OK) {
		res.data.data = parts;
		res.data.length = sizeof(ArrayList);
		FREE(allocator, res.data);
		BASE_ERROR_RESULT(res);
		return res;
	}

	res.data.data = parts;
	res.data.length = sizeof(ArrayList);
	res.status = ERROR_OK;
	return res;
}

Result standard_clone(Allocator *allocator, Slice ptr) {
	Result res;
	Slice new_mem;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.length == 0 || ptr.data == 0) {
		return res;
	}

	res = ALLOC(allocator, ptr.length);
	if (res.status != ERROR_OK) {
		return res;
	}
	if (res.data.length != ptr.length) {
		FREE(allocator, res.data);
		BASE_ERROR_RESULT(res);
		return res;
	}
	new_mem = res.data;

	res = slice_copy(new_mem, ptr);
	if (res.status != ERROR_OK) {
		FREE(allocator, new_mem);
		return res;
	}

	return res;
}

function Result raw_heap_alloc(Allocator* allocator, unsigned int length) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0) {
		return res;
	}

	res.data.data = malloc(length);
	if (res.data.data == 0) {
		return res;
	}
	res.data.length = length;
	res.status = ERROR_OK;

	return res;
}

function Result raw_heap_realloc(Allocator* allocator, Slice ptr, unsigned int length) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.length == 0 || ptr.data == 0 || length == 0) {
		return res;
	}

	res.data.data = realloc(ptr.data, length);
	if (res.data.data == 0) {
		return res;
	}
	res.data.length = length;
	res.status = ERROR_OK;

	return res;
}

function Result raw_heap_free(Allocator* allocator, Slice ptr) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.length == 0 || ptr.data == 0) {
		return res;
	}

	free(ptr.data);
	SET_NULL_SLICE(ptr);
	res.status = ERROR_OK;

	return res;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
function Result raw_heap_freeall(Allocator *allocator) {
	Result res;
	BASE_ERROR_RESULT(res);
	return res;
}
#pragma GCC diagnostic pop

function Result raw_heap_clone(Allocator *allocator, Slice ptr) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.length == 0 || ptr.data == 0) {
		return res;
	}

	res = ALLOC(allocator, ptr.length);
	if (res.status != ERROR_OK) {
		return res;
	}

	memcpy(res.data.data, ptr.data, ptr.length);
	return res;
}

function Allocator raw_heap_allocator = {
	raw_heap_alloc, raw_heap_realloc,
	raw_heap_free,  raw_heap_freeall,
	raw_heap_clone, standard_slice_split
};

Allocator *get_raw_heap_allocator(void) {
	return &raw_heap_allocator;
}
