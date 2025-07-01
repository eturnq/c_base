#ifndef _UTILITIES_QUEUE_H_
#define _UTILITIES_QUEUE_H_

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

#endif
