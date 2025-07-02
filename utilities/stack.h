#pragma once

#include "../utilities.h"

struct stack_collection_s {
	Linear outside_functions;
	Allocator *allocator;
	unsigned int item_size;
	unsigned int item_count;
	Slice buffer;
};
