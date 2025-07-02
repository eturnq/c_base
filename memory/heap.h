#pragma once

#include "../memory.h"

struct heap_allocator_s {
	Allocator outside_functions;
	Allocator *inside_functions;
};
