#pragma once

#include "../utilities.h"

struct queue_collection_s {
	Linear outside_functions;
	Allocator *allocator;
	Slice buffer;
	unsigned int head;
	unsigned int tail;
	unsigned int item_size;
	unsigned int item_count;
};
