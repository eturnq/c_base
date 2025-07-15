#include "../globals.h"
#include "../memory.h"
#include "../utilities.h"

function Result basic_linear_alloc(Allocator* allocator, unsigned int size) {
    Result res;
    BasicLinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    if (allocator == 0 || size == 0) {
        return res;
    }

    linear = (BasicLinearAllocator*) allocator;
    if (size > linear->current.length) {
        return res;
    }

    res.data.length = size;
    res.data = slice_sub(linear->current, 0, size);
    linear->current.length -= size;
    linear->current.data = (void*)((uint8_t*)linear->current.data + size);

    res.status = ERROR_OK;
    return res;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
function Result basic_linear_realloc(Allocator* allocator, Slice ptr, unsigned int size) {
    // TODO: implement more result statuses
    Result res;
    res.status = ERROR_OK;
		SET_NULL_SLICE(res.data);

    return res;
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
function Result basic_linear_free(Allocator* allocator, Slice ptr) {
    // TODO: implement more result statuses
    Result res;
    res.status = ERROR_OK;
		SET_NULL_SLICE(res.data);

    return res;
}
#pragma GCC diagnostic pop

function Result basic_linear_freeall(Allocator *allocator) {
    Result res;
    BasicLinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    if (allocator == 0) {
        return res;
    }

    linear = (BasicLinearAllocator*) allocator;
    linear->current = linear->buffer;

    res.status = ERROR_OK;

    return res;
}

function Result basic_linear_clone(Allocator *allocator, Slice ptr) {
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

Result new_basic_linear_allocator(Allocator* allocator, unsigned int max_size) {
    Result res;
    BasicLinearAllocator *linear;
    BASE_ERROR_RESULT(res);

    if (allocator == 0 || max_size == 0) {
        return res;
    }

    res = ALLOC(allocator, sizeof(BasicLinearAllocator));
    if (res.status != ERROR_OK) {
        return res;
    }
    if (res.data.length != sizeof(BasicLinearAllocator)) {
        FREE(allocator, res.data);
        BASE_ERROR_RESULT(res);
        return res;
    }
    linear = (BasicLinearAllocator*) res.data.data;

    res = ALLOC(allocator, max_size);
    if (res.status != ERROR_OK) {
        return res;
    }
    if (res.data.length != max_size) {
        FREE(allocator, res.data);
        res.data.data = linear;
        res.data.length = sizeof(BasicLinearAllocator);
        FREE(allocator, res.data);
        BASE_ERROR_RESULT(res);
        return res;
    }
    linear->buffer = res.data;
    linear->current = res.data;
    linear->inside_methods = allocator;

    linear->outside_methods.alloc = basic_linear_alloc;
    linear->outside_methods.realloc = basic_linear_realloc;
    linear->outside_methods.free = basic_linear_free;
    linear->outside_methods.freeall = basic_linear_freeall;
    linear->outside_methods.clone = basic_linear_clone;
    linear->outside_methods.slice_split = standard_slice_split;

    res.data.data = linear;
    res.data.length = sizeof(BasicLinearAllocator);
    return res;
}

Result deinit_basic_linear_allocator(BasicLinearAllocator* linear) {
    Result res;
    BASE_ERROR_RESULT(res);

    if (linear == 0) {
        return res;
    }

    res = FREE(linear->inside_methods, linear->buffer);
    if (res.status != ERROR_OK) {
        return res;
    }

    res.data.data = linear;
    res.data.length = sizeof(BasicLinearAllocator);
    res = FREE(linear->inside_methods, res.data);
    if (res.status != ERROR_OK) {
        return res;
    }

    res.status = ERROR_OK;
    return res;
}

#define SET_LINEAR_BLOCK(block, ptr) \
block->mem = ptr;\
block->next = 0

typedef struct linear_block_s LinearBlock;
struct linear_block_s {
	Slice mem;
	LinearBlock *next;
};

typedef struct {
	Allocator outside_methods;
	Allocator *inside_methods;
	Slice buffer;
	LinearBlock *available;
} LinearAllocator;

function Result linear_allocation_from_block(LinearBlock *self, unsigned int size) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (self->mem.length >= size) {
		res.data.length = size;
		res.data.data = self->mem.data;
		res.status = ERROR_OK;

		self->mem.data = (void*)((uint8_t *)self->mem.data + size);
		self->mem.length -= size;
	}

	return res;
}

function Result linear_alloc(Allocator *allocator, unsigned int size) {
	Result res;
	LinearAllocator *self;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || size == 0) {
		return res;
	}

	self = (LinearAllocator*) allocator;
	LinearBlock *current = self->available;
	while (current != 0) {
		if (current->mem.length >= size) {
			return linear_allocation_from_block(current, size);
		}
		current = current->next;
	}
	
	return res;
}

function Result linear_free(Allocator *allocator, Slice ptr) {
	Result res;
	LinearAllocator *self;
	uint8_t *buffer_min, *buffer_max, *ptr_min, *ptr_max;
	uint8_t *current_min, *current_max;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || ptr.length == 0 || ptr.data == 0) {
		return res;
	}

	self = (LinearAllocator *) allocator;
	ptr_min = (uint8_t*) ptr.data;
	ptr_max = ptr_min + ptr.length;
	buffer_min = (uint8_t*) self->buffer.data;
	buffer_max = buffer_min + self->buffer.length;

	if (ptr_min < buffer_min || ptr_max > buffer_max) {
		return res;
	}

	LinearBlock *current = self->available;
	current_min = (uint8_t *) current->mem.data;
	current_max = current_min + current->mem.length;
	while (current != 0) {
		if (current_min == ptr_max) {
			// ptr is to the left of current
			current->mem.data = ptr.data;
			current->mem.length += ptr.length;
			res.status = ERROR_OK;
			return res;
		} else if (current_max == ptr_min) {
			// ptr is to the right of current
			current->mem.length += ptr.length;
			res.status = ERROR_OK;
			return res;
		}
		current = current->next;
	}
	// ptr does not fit nicely into any other blocks

	LinearBlock *new_block;
	current = self->available;
	res = ALLOC(allocator, sizeof(LinearBlock));
	if (res.status != ERROR_OK) {
		return res;
	}
	if (res.data.length != sizeof(LinearBlock)) {
		// TODO: this case will leak memory!
		BASE_ERROR_RESULT(res);
		return res;
	}

	new_block = (LinearBlock *) res.data.data;
	BASE_ERROR_RESULT(res);
	SET_LINEAR_BLOCK(new_block, ptr);

	while (current->next != 0) {
		current_min = (uint8_t *) current->mem.data;
		current_max = current_min + current->mem.length;
		if (ptr_min < current_min) {
			self->available = new_block;
			new_block->next = current;
			res.status = ERROR_OK;
			return res;
		}

		current = current->next;
	}

	current_min = (uint8_t *) current->mem.data;
	current_max = current_min + current->mem.length;
	if (ptr_min < current_min) {
		self->available = new_block;
		new_block->next = current;
		res.status = ERROR_OK;
		return res;
	}

	if (ptr_min > (uint8_t *) current->mem.data) {
		new_block->next = 0;
		current->next = new_block;
		res.status = ERROR_OK;
		return res;
	}

	BASE_ERROR_RESULT(res);
	return res;
}

function Result linear_freeall(Allocator *allocator) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0) {
		return res;
	}

	LinearAllocator *self = (LinearAllocator *) allocator;
	self->available->mem.length = self->buffer.length - sizeof(LinearBlock);
	self->available->mem.data = (void*)((uint8_t *)self->buffer.data + sizeof(LinearBlock));
	self->available->next = 0;

	res.status = ERROR_OK;
	return res;
}

Result init_linear_allocator(Allocator* allocator, unsigned int max_size) {
    Result res;
		LinearAllocator *self;
		unsigned int buffer_size;
    BASE_ERROR_RESULT(res);

    if (allocator == 0 || max_size == 0) {
        return res;
    }

		res = ALLOC(allocator, sizeof(LinearAllocator));
		if (res.status != ERROR_OK) {
			return res;
		}
		if (res.data.length != sizeof(LinearAllocator)) {
			FREE(allocator, res.data);
			BASE_ERROR_RESULT(res);
			return res;
		}

		self = (LinearAllocator *) res.data.data;
		self->inside_methods = allocator;

		// Get the buffer
		buffer_size = max_size + sizeof(LinearBlock); 
		res = ALLOC(allocator, buffer_size);
		if (res.status != ERROR_OK) {
			res.data.data = self;
			res.data.length = sizeof(LinearAllocator);
			FREE(allocator, res.data);
			return res;
		}
		if (res.data.length != buffer_size) {
			FREE(allocator, res.data);
			res.data.data = self;
			res.data.length = sizeof(LinearAllocator);
			FREE(allocator, res.data);
			BASE_ERROR_RESULT(res);
			return res;
		}
		self->buffer = res.data;

		LinearBlock *low_block = (LinearBlock *) self->buffer.data;
		low_block->mem.length = max_size;
		low_block->mem.data = (void*)&low_block[1];
		low_block->next = 0;
		self->available = low_block;

		self->outside_methods.alloc = linear_alloc;
		self->outside_methods.free = linear_free;
		self->outside_methods.realloc = standard_realloc;
		self->outside_methods.freeall = linear_freeall;
		self->outside_methods.clone = standard_clone;
		self->outside_methods.slice_split = standard_slice_split;

		// Linear allocator created successfully.
		res.status = ERROR_OK;
		res.data.length = sizeof(LinearAllocator);
		res.data.data = (void*)self;
    return res;
}

Result deinit_linear_allocator(Allocator* linear) {
    Result res;
		LinearAllocator *self;
    BASE_ERROR_RESULT(res);

    if (linear == 0) {
        return res;
    }

		self = (LinearAllocator *) linear;
	
		res = FREE(self->inside_methods, self->buffer);
		if (res.status != ERROR_OK)	{
			return res;
		}

		res.data.data = (void*) self;
		res.data.length = sizeof(LinearAllocator);
		res = FREE(self->inside_methods, res.data);
		return res;
}
