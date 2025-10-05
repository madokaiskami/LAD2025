#!/bin/sh
# POSIX / dash-compatible ASCII-art random painter
# Usage: ./randomize.sh [delay_seconds]
#   delay_seconds: optional float (e.g., 0.02). Default 0 (no delay).

set -eu

DELAY="${1:-0}"
export LC_ALL=C

TMP_IN="$(mktemp)"
TMP_COORDS="$(mktemp)"

cleanup() {
  rm -f "$TMP_IN" "$TMP_COORDS" 2>/dev/null || true
  if rows_cols="$(stty size </dev/tty 2>/dev/null)"; then
    set -- $rows_cols
    rows="$1"
    if [ "$rows" -gt 0 ] 2>/dev/null; then
      rows=$((rows - 1))
      tput cup "$rows" 0 2>/dev/null || true
    fi
  fi
}
trap cleanup EXIT INT TERM HUP

cat >"$TMP_IN"

tput clear 2>/dev/null || true


x=0 y=0
od -An -t u1 -v -w1 "$TMP_IN"     | tr -d ' \t'     | while IFS= read -r b; do
    [ -n "$b" ] || continue
    if [ "$b" -eq 10 ] 2>/dev/null; then
      y=$((y + 1))
      x=0
      continue
    fi
    if [ "$b" -eq 13 ] 2>/dev/null; then
      continue
    fi
    printf '%s %s %s\n' "$y" "$x" "$b"
    x=$((x + 1))
  done >"$TMP_COORDS"

shuf "$TMP_COORDS" | while IFS=' ' read -r Y X B; do
  tput cup "$Y" "$X" 2>/dev/null || true

  FMT="$(printf '\\%03o' "$B")"

  printf "$FMT"

  if [ "$DELAY" != "0" ]; then

    sleep "$DELAY"
  fi
done

