09_PatchDiff — Linux Application Development 2025 homework

Contents
--------

- maze.c       : base version of the maze generator (fixed 6x6, '.' and '#')
- patch1.diff  : unified patch to make the maze size configurable via argv[1]
- patch2.diff  : unified patch to change argv[1] to "passage+wall" chars and argv[2] to size
- patch3.diff  : unified patch to add deterministic RNG seed as argv[1], with argv[2]="passage+wall", argv[3]=size
- Makefile     : builds 4 binaries and provides run/clean targets

Targets
-------

- `make` or `make all`
    Builds:
      maze0 : compiled directly from maze.c
      maze1 : maze.c + patch1.diff
      maze2 : maze.c + patch1.diff + patch2.diff
      maze3 : maze.c + patch1.diff + patch2.diff + patch3.diff

- `make run`
    Runs all four binaries with sample parameters, printing one maze per variant.

- `make clean`
    Removes generated sources (maze1.c, maze2.c, maze3.c) and binaries.
