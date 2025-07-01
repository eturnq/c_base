#ifndef _UTILITIES_STACK_H_
#define _UTILITIES_STACK_H_

#include "../utilities.h"

struct stack_collection_s {
	Collection outside_functions;
	Allocator *allocator;
	unsigned int item_size;
	unsigned int item_count;
	Slice buffer;
};

#endif
