CFLAGS=-std=c11 -g -static

thisiscc: thisiscc.c

test: thisiscc
	./test.sh

clean:
	rm -f thisiscc *.o *~ tmp*

.PHONY: test clean
