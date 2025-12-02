#!/bin/sh
set -e

autoreconf --install --force

echo
echo "Now run:"
echo "    ./configure"
echo "    make"
echo "    make check"
