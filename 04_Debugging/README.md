# 04_Debugging assignment

Files:
- `range.c` – arithmetic progression generator (range-like).
- `script1.gdb` – prints vars when current element is divisible by 5 for `./range 1 12`.
- `script2.gdb` – prints vars for elements #28..#35 (1-based) for `./range -100 100 3`.
- `expected1.txt`, `expected2.txt` – expected outputs for the scripts.
- `Makefile` – builds with `-O0 -g`; `make test` runs the gdb scripts and compares filtered output; `make clean` removes build artifacts.

## Build
```sh
make
```

## Test
```sh
make test
```

The scripts redirect the program's own output to `/dev/null`, emit stable lines starting with `@@@` using `printf` inside GDB, and `grep` those lines before comparison.