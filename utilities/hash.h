#pragma once

#include "../utilities.h"

#define MAX_OFFSET 255
#define SLOT_EMPTY 256
#define ITEM_REMOVED 257

struct hashmap8_s {
	Map outside_functions;
	ArrayList data;
	StackCollection removals;
	Slice (*hash)(Hashmap8 *, Slice);
	unsigned int offsets[MAX_OFFSET + 1];
};
