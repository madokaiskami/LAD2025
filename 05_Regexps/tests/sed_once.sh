#!/usr/bin/env bash
# tests/sed_once.sh — emulate: echo "$3" | sed -E "s/$1/$2/" with minimal escaping
set -euo pipefail
re="$1"; sub="$2"; s="$3"
# Escape the delimiter '/' to avoid collisions
re_esc=${re//\//\\/}
sub_esc=${sub//\//\\/}
printf "%s\n" "$s" | sed -E "s/${re_esc}/${sub_esc}/"
