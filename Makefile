LIB=base.a
MEM_OBJS=memory/heap.o
UTIL_OBJS=utilities/arraylist.o utilities/hash.o utilities/queue.o utilities/slice.o utilities/stack.o
OBJS=$(MEM_OBJS) $(UTIL_OBJS)
CFLAGS=-Wall -Wextra
LDFLAGS=-L. -l:$(LIB)

$(LIB): $(OBJS)
	$(AR) rcs $(LIB) $(OBJS)

test: test.c $(LIB)
	$(CC) -o test test.c $(LDFLAGS)
	./test

clean:
	rm -f test $(LIB) $(OBJS)
.PHONY: clean
