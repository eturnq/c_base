#ifndef _UTILITIES_H_
#define _UTILITIES_H_

#include "memory.h"

#include <stdint.h>

typedef struct allocator_s Allocator;

typedef struct slice_s Slice;
struct slice_s {
	unsigned int length;
	void *data;
};

#define IS_NULL_SLICE(slice) (slice.length == 0 || slice.data == 0)

uint8_t hash8_slice(Slice);

enum error_code {
	ERROR_OK,
	ERROR_ERR,
};

typedef struct result_s Result;
struct result_s {
	enum error_code status;
	Slice data;
};

#define BASE_ERROR_RESULT(result) \
	result.status = ERROR_ERR; \
	result.data.length = 0; \
	result.data.data = 0

typedef struct map_s Map;
struct map_s {
	Result (*add)(Map*, Slice, Slice);
	Result (*get)(Map*, Slice);
	Result (*remove)(Map*, Slice);
};

typedef struct hashmap8_s Hashmap8;

#define MAP_ADD(map, key, value) (((Map*)map)->add(map, key, value))
#define MAP_GET(map, key) (((Map*)map)->get(map, key))
#define MAP_REMOVE(map, key) (((Map*)map)->remove(map, key))


typedef struct collection_s Collection;
struct collection_s {
	Result (*push)(Collection*, Slice);
	Result (*pop)(Collection*);
};

#define COLLECTION_PUSH(collection, item) (((Collection*)collection)->push(collection, item))
#define COLLECTION_POP(collection) (((Collection*)collection)->pop(collection))

typedef struct stack_collection_s StackCollection;
Result new_stack_collection(Allocator*, unsigned int, unsigned int);

#endif
