LIB=base.a
MEM_OBJS=memory/heap.o
UTIL_OBJS=utilities/arraylist.o utilities/hash.o utilities/queue.o utilities/slice.o utilities/stack.o
TEST_OBJS=memory/heap_test.o utilities/arraylist_test.o utilities/queue_test.o utilities/slice_test.o utilities/stack_test.o
OBJS=$(MEM_OBJS) $(UTIL_OBJS)
CFLAGS=-Wall -Wextra
LDFLAGS=-L. -l:$(LIB)

$(LIB): $(OBJS)
	$(AR) rcs $(LIB) $(OBJS)

test: test.c $(LIB) $(TEST_OBJS)
	$(CC) -o test test.c $(TEST_OBJS) $(LDFLAGS)
	./test

cleanlib:
	rm -f $(LIB) $(OBJS)
.PHONY: cleanlib

cleantest:
	rm -f test $(TEST_OBJS)
.PHONY: cleantest

clean: cleanlib cleantest
.PHONY: clean
