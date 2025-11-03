#!/usr/bin/env bash
set -euo pipefail
autoreconf --install --force --verbose
echo "Now run: ./configure && make && make check"
