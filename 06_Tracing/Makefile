
CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -std=c11
LDFLAGS ?=

all: move libprotect.so

move: move.c
	$(CC) $(CFLAGS) -o $@ $<

libprotect.so: protect.c
	$(CC) -shared -fPIC -o $@ $< -ldl

.PHONY: test clean

test: all
	@./run_tests.sh

clean:
	@rm -f move libprotect.so
	@rm -f *.o
	@echo "cleaned."
