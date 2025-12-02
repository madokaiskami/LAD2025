CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -std=c11

ALL_BINS = maze0 maze1 maze2 maze3

all: $(ALL_BINS)

maze0: maze.c
	$(CC) $(CFLAGS) -o $@ $<

maze1.c: maze.c patch1.diff
	cp maze.c maze1.c
	patch maze1.c patch1.diff

maze2.c: maze1.c patch2.diff
	cp maze1.c maze2.c
	patch maze2.c patch2.diff

maze3.c: maze2.c patch3.diff
	cp maze2.c maze3.c
	patch maze3.c patch3.diff

maze1: maze1.c
	$(CC) $(CFLAGS) -o $@ maze1.c

maze2: maze2.c
	$(CC) $(CFLAGS) -o $@ maze2.c

maze3: maze3.c
	$(CC) $(CFLAGS) -o $@ maze3.c

run: all
	@echo "=== maze0 (fixed 6x6, '.' '#') ==="
	./maze0
	@echo
	@echo "=== maze1 (SIZE from argv[1]) ==="
	./maze1 6
	@echo
	@echo "=== maze2 (PW SIZE) ==="
	./maze2 ".#" 6
	@echo
	@echo "=== maze3 (SEED PW SIZE) ==="
	./maze3 123 ".#" 6

clean:
	rm -f $(ALL_BINS) maze1.c maze2.c maze3.c

.PHONY: all run clean
