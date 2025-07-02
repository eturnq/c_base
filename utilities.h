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

int slice_cmp(Slice, Slice);
Slice slice_sub(Slice a, int offset, unsigned int length);

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

#define MAP_ADD(map, key, value) (((Map*)map)->add((Map*)map, key, value))
#define MAP_GET(map, key) (((Map*)map)->get((Map*)map, key))
#define MAP_REMOVE(map, key) (((Map*)map)->remove((Map*)map, key))


typedef struct linear_s Linear;
struct linear_s {
	Result (*push)(Linear*, Slice);
	Result (*pop)(Linear*);
};

#define LINEAR_PUSH(collection, item) (((Linear*)collection)->push((Linear*)collection, item))
#define LINEAR_POP(collection) (((Linear*)collection)->pop((Linear*)collection))

typedef struct stack_collection_s StackCollection;
Result new_stack_collection(Allocator*, unsigned int, unsigned int);
Result deinit_stack_collection(StackCollection *stack);

typedef struct queue_collection_s QueueCollection;
Result new_queue_collection(Allocator*, unsigned int, unsigned int);
Result deinit_queue_collection(QueueCollection*);

typedef struct indexing_s Indexing;
struct indexing_s {
	Linear linear_functions;
	Result (*get)(Indexing*, int);
	Result (*index_of)(Indexing*, Slice);
	Result (*remove)(Indexing*, int);
	Result (*insert)(Indexing*, Slice, int);
	Result (*swap)(Indexing*, int, int);
	Result (*replace)(Indexing*, Slice, int);
};

typedef struct array_list_s ArrayList;
Result new_array_list(Allocator*, unsigned int, unsigned int);
Result deinit_array_list(ArrayList*);

#endif
