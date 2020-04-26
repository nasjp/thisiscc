CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

thisiscc: $(OBJS)
	$(CC) -o thisiscc $(OBJS) $(LDFLAGS)

$(OBJS): thisiscc.h

test: thisiscc
	./test.sh

clean:
	rm -f thisiscc *.o *~ tmp*

.PHONY: test clean
