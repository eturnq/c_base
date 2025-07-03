LIB=base.a
MEM_OBJS=memory/heap.o
MEM_TEST_OBJS=$(MEM_OBJS:.o=_test.o)
UTIL_OBJS=utilities/arraylist.o utilities/hash.o utilities/queue.o utilities/slice.o utilities/stack.o
UTIL_TEST_OBJS=$(UTIL_OBJS:.o=_test.o)
TEST_OBJS=$(MEM_TEST_OBJS) $(UTIL_TEST_OBJS)
OBJS=$(MEM_OBJS) $(UTIL_OBJS)
CFLAGS=-Wall -Wextra -g
LDFLAGS=-L. -l:$(LIB)

$(LIB): $(OBJS)
	$(AR) rcs $(LIB) $(OBJS)

test: test.c $(LIB) $(TEST_OBJS)
	$(CC) -o test test.c $(TEST_OBJS) $(CFLAGS) $(LDFLAGS)
	./test

cleanlib:
	rm -f $(LIB) $(OBJS)
.PHONY: cleanlib

cleantest:
	rm -f test $(TEST_OBJS)
.PHONY: cleantest

clean: cleanlib cleantest
.PHONY: clean
