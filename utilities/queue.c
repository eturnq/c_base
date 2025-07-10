#include "../globals.h"
#include "../memory.h"

#include <stdint.h>
#include <string.h>

function Result queue_push(Linear* linear, Slice item) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (linear == 0 || item.length == 0 || item.data == 0) {
		return res;
	}

	QueueCollection *queue = (QueueCollection*)linear;
	if (item.length != queue->item_size) {
		return res;
	}

	if (queue->item_count * queue->item_size >= queue->buffer.length) {
		unsigned int relocation_start = queue->head * queue->item_size;
		unsigned int relocation_length = queue->buffer.length - relocation_start;
		Result realloc_res = REALLOC(queue->allocator, queue->buffer, queue->buffer.length << 1);
		if (realloc_res.status != ERROR_OK) {
			return res;
		}
		queue->buffer = realloc_res.data;
		queue->head = (queue->buffer.length - relocation_length) / queue->item_size;
		memcpy(
			(uint8_t*)queue->buffer.data + (queue->buffer.length - relocation_length),
			(uint8_t*)queue->buffer.data + relocation_start,
			relocation_length
		);
	}

	memcpy((uint8_t*)queue->buffer.data + (queue->tail * queue->item_size), item.data, item.length);
	queue->tail++;
	queue->item_count++;

	if (queue->tail * queue->item_size >= queue->buffer.length) {
		queue->tail = 0;
	}

	res.status = ERROR_OK;
	return res;
}

function Result queue_pop(Linear *linear) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (linear == 0) {
		return res;
	}

	QueueCollection *queue = (QueueCollection*) linear;

	if (queue->head == queue->tail) {
		return res;
	}

	if (queue->head * queue->item_size == queue->buffer.length) {
		queue->head = 0;
	}

	res.status = ERROR_OK;
	res.data.length = queue->item_size;
	res.data.data = (uint8_t*)queue->buffer.data + (queue->head * queue->item_size);
	queue->head++;

	return res;
}

function Result queue_clone(Linear *linear) {
    	QueueCollection *self;
	Result res;
	BASE_ERROR_RESULT(res); 

	if (linear == 0) {
		return res;
	}

	self = (QueueCollection *) linear;

	res.data.data = self;
	res.data.length = sizeof(QueueCollection);
	res = CLONE(self->allocator, res.data);
	if (res.status != ERROR_OK) {
		return res;
	}
	if (res.data.length != sizeof(QueueCollection)) {
		FREE(self->allocator, res.data);
		BASE_ERROR_RESULT(res); 
		return res;
	}

	return res;
}

Result new_queue_collection(QueueCollection* queue, Allocator* allocator, unsigned int item_size, unsigned int max_count) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (allocator == 0 || item_size == 0 || max_count == 0) {
		return res;
	}

	queue->allocator = allocator;
	queue->item_size = item_size;
	queue->item_count = 0;
	queue->head = 0;
	queue->tail = 0;

	Result buffer_res = ALLOC(allocator, max_count * item_size);
	if (buffer_res.status != ERROR_OK) {
		return res;
	}

	queue->buffer = buffer_res.data;
	queue->outside_functions.clone = queue_clone;
	queue->outside_functions.pop = queue_pop;
	queue->outside_functions.push = queue_push;

	res.status = ERROR_OK;
	res.data.length = sizeof(QueueCollection);
	res.data.data = &queue;

	return res;
}

Result deinit_queue_collection(QueueCollection* queue) {
	Result res;
	BASE_ERROR_RESULT(res);

	if (queue == 0) {
		return res;
	}

	res = FREE(queue->allocator, queue->buffer);
	if (res.status != ERROR_OK) {
		return res;
	}

	queue->head = 0;
	queue->tail = 0;
	queue->item_count = 0;
	queue->item_size = 0;

	Slice s = { queue, sizeof(QueueCollection) };
	res.data = s;

	return res;
}
