#pragma once

#include "../utilities.h"
//#include "../memory.h"

struct linear_alloc_s {
  Allocator outside_methods;
  Allocator *inside_methods;
  Slice buffer;
  unsigned int position;
  ArrayList free_blocks;
};
