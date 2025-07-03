#pragma once
#include "memory.h"

#include <stdint.h>

typedef struct allocator_s Allocator;

typedef struct slice_s {
	unsigned int length;
	void *data;
} Slice;

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

#define RESULT_UNWRAP(result, type) (*((type*)result.data.data))

int slice_cmp(Slice, Slice);
Slice slice_sub(Slice a, int offset, unsigned int length);
Result slice_copy(Slice a, Slice b);

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
struct stack_collection_s {
	Linear outside_functions;
	Allocator *allocator;
	unsigned int item_size;
	unsigned int item_count;
	Slice buffer;
};
Result new_stack_collection(Allocator*, unsigned int item_size, unsigned int max_count);
Result deinit_stack_collection(StackCollection *stack);

typedef struct queue_collection_s QueueCollection;
#include "utilities/queue.h"
Result new_queue_collection(Allocator*, unsigned int item_size, unsigned int max_count);
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

#define INDEXING_GET(collection, index) (((Indexing*)collection)->get((Indexing*)collection, index))
#define INDEXING_INDEX_OF(collection, ptr) (((Indexing*)collection)->index_of((Indexing*)collection, ptr))
#define INDEXING_REMOVE(collection, index) (((Indexing*)collection)->remove((Indexing*)collection, index))
#define INDEXING_INSERT(collection, ptr, index) (((Indexing*)collection)->insert((Indexing*)collection, ptr, index))
#define INDEXING_SWAP(collection, a, b) (((Indexing*)collection)->swap((Indexing*)collection, a, b))
#define INDEXING_REPLACE(collection, ptr, index) (((Indexing*)collection)->replace((Indexing*)collection, ptr, index))

typedef struct array_list_s ArrayList;
struct array_list_s {
	Indexing outside_functions;
	Allocator* allocator;
	Slice buffer;
	unsigned int item_size;
	unsigned int item_count;
};
Result new_array_list(Allocator*, unsigned int item_size, unsigned int max_count);
Result deinit_array_list(ArrayList*);

typedef struct hashmap8_s Hashmap8;
#include "utilities/hash.h"
Result new_hashmap8(Allocator*);
Result deinit_hashmap8(Hashmap8*);
