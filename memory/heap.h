#ifndef _MEMORY_HEAP_H
#define _MEMORY_HEAP_H

struct heap_allocator_s {
	Allocator outside_functions;
	Allocator *inside_functions;
};

#endif
