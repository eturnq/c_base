LIB=base.a
OBJS=memory/heap.o utilities/hash.o utilities/queue.o utilities/slice.o utilities/stack.o
CFLAGS=-Wall -Wextra
LDFLAGS=-L. -l:$(LIB)

$(LIB): $(OBJS)
	$(AR) rcs $(LIB) $(OBJS)

test: test.c $(LIBS)
	$(CC) -o test test.c $(LDFLAGS)
	./test

clean:
	rm -f test $(LIB) $(OBJS)
.PHONY: clean
