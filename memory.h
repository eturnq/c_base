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
	Result (*slice_split)(Allocator *, Slice whole, Slice part);
};

#define ALLOC(allocator, length) (((Allocator*)allocator)->alloc(allocator, length))
#define REALLOC(allocator, ptr, length) (((Allocator*)allocator)->realloc(allocator, ptr, length))
#define FREE(allocator, ptr) (((Allocator*)allocator)->free(allocator, ptr))
#define FREEALL(allocator) (((Allocator*)allocator)->freeall(allocator))
#define CLONE(allocator, ptr) (((Allocator*)allocator)->clone(allocator, ptr))
#define SLICE_SPLIT(allocator, whole, part) (((Allocator*)allocator)->slice_split(allocator, whole, part))

Result standard_clone(Allocator *allocator, Slice ptr);
Result standard_realloc(Allocator *allocator, Slice ptr, unsigned int size);
Result standard_slice_split(Allocator *allocator, Slice whole, Slice part);

Allocator *get_raw_heap_allocator(void);

struct heap_allocator_s;
typedef struct heap_allocator_s HeapAllocator;

struct basic_linear_alloc_s;
typedef struct basic_linear_alloc_s BasicLinearAllocator;
Result new_basic_linear_allocator(Allocator*, unsigned int max_size);
Result deinit_basic_linear_allocator(BasicLinearAllocator*);

Result init_linear_allocator(Allocator*, unsigned int max_size);
Result deinit_linear_allocator(Allocator*);

#include "memory/heap.h"
#include "memory/linear_alloc.h"
