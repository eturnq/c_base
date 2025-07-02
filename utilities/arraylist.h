#pragma once

#include "../utilities.h"

struct array_list_s {
	Indexing outside_functions;
	Allocator* allocator;
	Slice buffer;
	unsigned int item_size;
	unsigned int item_count;
};
