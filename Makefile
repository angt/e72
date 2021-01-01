CC     = cc
CFLAGS = -Wall -O3 -DE72_TEST=1

test:
	$(CC) $(CFLAGS) -o e72 hash.c

clean:
	rm -f e72

.PHONY: test clean
