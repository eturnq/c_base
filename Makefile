LIB=base.a
OBJS=memory/heap.o utilities/hash.o utilities/queue.o utilities/slice.o utilities/stack.o

$(LIB): $(OBJS)
	$(AR) -rs $(LIB) $(OBJS)

test: $(LIB) test.c
	$(CC) -o test $>
	./test

clean:
	rm -f test $(LIB) $(OBJS)
.PHONY: clean
