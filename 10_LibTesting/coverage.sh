#!/bin/sh
# Primitive coverage helper: rebuild with gcov flags and print coverage
set -e

# Clean previous builds if any
if [ -f Makefile ]; then
    make distclean >/dev/null 2>&1 || true
fi

CFLAGS="--coverage -O0 -g" LDFLAGS="--coverage" ./configure

make check

# Run gcov inside src/ on the compiled object base
cd src
: "${GCOV:=gcov}"
"$GCOV" -o .libs buf.c
