CC=gcc
CFLAGS=-Wall -Werror -Wvla -std=gnu11 -fsanitize=address
PFLAGS=-fprofile-arcs -ftest-coverage
DFLAGS=-g
HEADERS=server.h
SRC=server.c

procchat: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(DFLAGS) $(SRC) -o $@

test_gevent: test_gevent.c
	$(CC) $(CFLAGS) $(DFLAGS) $(SRC) -o $@

test_connect1: test_connect1.c
	$(CC) $(CFLAGS) $(DFLAGS) $(SRC) -o $@

create_test: $(SRC) $(HEADERS)
	$(CC) $(CFLAGS) $(DFLAGS) $(SRC) -o test

test: create_test
	bash test.sh

clean:
	rm -f procchat
	rm test_gevent
	rm test_connect
	rm test
	rm gevent
	rm -r dom
