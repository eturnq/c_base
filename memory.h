#pragma once
#include "utilities.h"

typedef struct allocator_s Allocator;
typedef struct result_s Result;
typedef struct slice_s Slice;
struct allocator_s {
	Result (*alloc)(Allocator*, unsigned int);
	Result (*realloc)(Allocator*, Slice, unsigned int);
	Result (*free)(Allocator*, Slice);
	Result (*freeall)(Allocator*);
	Result (*clone)(Allocator*, Slice);
};

#define ALLOC(allocator, length) (((Allocator*)allocator)->alloc(allocator, length))
#define REALLOC(allocator, ptr, length) (((Allocator*)allocator)->realloc(allocator, ptr, length))
#define FREE(allocator, ptr) (((Allocator*)allocator)->free(allocator, ptr))
#define FREEALL(allocator) (((Allocator*)allocator)->freeall(allocator))
#define CLONE(allocator, ptr) (((Allocator*)allocator)->clone(allocator, ptr))

Allocator *get_raw_heap_allocator(void);

struct heap_allocator_s;
typedef struct heap_allocator_s HeapAllocator;

#include "memory/heap.h"
