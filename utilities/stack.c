#include "../globals.h"
#include "../memory.h"
#include "../utilities.h"

#include <stdint.h>
#include <string.h>

function Result stack_push(Linear *collection, Slice item) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (collection == 0 || item.length == 0 || item.data == 0) {
		return res;
	}

	StackCollection *stack = (StackCollection*)collection;
	if (stack->item_size != item.length) {
		return res;
	}

	unsigned int new_offset = stack->item_size * stack->item_count;
	unsigned int new_length = stack->item_size + new_offset;

	if (new_length > stack->buffer.length) {
		Result result = REALLOC(stack->allocator, stack->buffer, stack->buffer.length << 1);
		if (result.status == ERROR_ERR) {
			return res;
		}
		stack->buffer = result.data;
	}

	memcpy((char*)stack->buffer.data + new_offset, item.data, stack->item_size);
	stack->item_count++;
	res.status = ERROR_OK;

	return res;
}

function Result stack_pop(Linear *collection) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (collection == 0) {
		return res;
	}

	StackCollection* stack = (StackCollection*) collection;
	if (stack->item_count < 1) {
		return res;
	}

	stack->item_count--;
	unsigned int new_offset = stack->item_count * stack->item_size;

	res.status = ERROR_OK;
	res.data.length = stack->item_size;
	res.data.data = (void*) ((uint8_t*)stack->buffer.data + new_offset);

	return res;
}

function Result stack_clone(Linear *collection) {
	Result res;
	StackCollection *self;
	BASE_ERROR_RESULT(res); 

	if (collection == 0) {
		return res;
	}

	self = (StackCollection *) collection;
	res.data.data = self;
	res.data.length = sizeof(StackCollection);

	res = CLONE(self->allocator, res.data);
	if (res.status != ERROR_OK) {
		return res;
	}
	if (res.data.length != sizeof(StackCollection)) {
		FREE(self->allocator, res.data);
		BASE_ERROR_RESULT(res);
		return res;
	}

	return res;
}

Result new_stack_collection(StackCollection* stack, Allocator* allocator, unsigned int item_size, unsigned int initial_length) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || item_size == 0 || initial_length == 0) {
		return res;
	}

	stack->outside_functions.clone = stack_clone;
	stack->outside_functions.pop = stack_pop;
	stack->outside_functions.push = stack_push;

	stack->allocator = allocator;
	stack->item_size = item_size;
	stack->item_count = 0;
	Result alloc_res = ALLOC(allocator, initial_length * item_size);
	if (alloc_res.status == ERROR_ERR) {
		return res;
	}
	stack->buffer = alloc_res.data;

	res.data.length = sizeof(StackCollection);
	res.data.data = stack;
	res.status = ERROR_OK;

	return res;
}

Result deinit_stack_collection(StackCollection *stack) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (stack == 0) {
		return res;
	}

	Result dealloc_res = FREE(stack->allocator, stack->buffer);
	if (dealloc_res.status != ERROR_OK) {
		return res;
	}

	stack->item_size = 0;
	stack->item_count = 0;

	res.status = ERROR_OK;
	return res;
}
