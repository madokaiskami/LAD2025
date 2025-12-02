# 10_LibTesting — Growable buffer library with tests

This is a homework solution for the Linux Application Development 2025 course,
task "Библиотеки и тестирование" / 10_LibTesting.

It turns the public-domain "Growable Memory Buffers for C99" idea into a
classic C library with:

* a single C function (`buf__grow_raw`) and the rest as macros in `buf.h`;
* a shared/static library built via Autotools + libtool;
* unit tests written with libcheck;
* a simple coverage helper script based on `gcov`.

## Requirements

On ALT Linux you will need (as suggested in the task):

* `make`, `automake`, `autoconf`
* `libtool`
* `pkg-config`
* `libcheck-devel`, `check`
* `gcc` (with gcov support, usually packaged as `gcc`)

## Building

```sh
./autogen.sh
./configure
make
```

## Running tests

```sh
make check              # run libcheck-based tests
CK_VERBOSITY=verbose make check   # more detailed output
```

## Coverage

A primitive coverage helper is provided:

```sh
./coverage.sh
```

It will:

1. reconfigure the project with `--coverage` flags;
2. run the tests (`make check`);
3. run `gcov` on `src/buf.c` and print line coverage statistics.

## Installation (optional)

```sh
make install
```

This will install the libtool library `libgrowablebuf` and the public header
`buf.h` into the usual prefix (by default `/usr/local` or as passed to
`./configure --prefix=...`).

## Layout

* `configure.ac`, `Makefile.am`, `autogen.sh` — Autotools/libtool plumbing
* `src/buf.c`, `src/buf.h` — library implementation and public header
* `tests/test_buf.c` — libcheck test suite
* `coverage.sh` — simple gcov-based coverage helper
