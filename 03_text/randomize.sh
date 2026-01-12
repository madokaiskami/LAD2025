#!/bin/sh

# dash/POSIX sh compatible
# Usage: ./randomize.sh [delay_seconds]

DELAY=${1:-0}

# temp files
TMP_IN=$(mktemp)
TMP_U16=$(mktemp)
TMP_OD=$(mktemp)
TMP_LIST=$(mktemp)
TMP_SHUF=$(mktemp)

cleanup() {
  rm -f "$TMP_IN" "$TMP_U16" "$TMP_OD" "$TMP_LIST" "$TMP_SHUF" 2>/dev/null || true
  tput cnorm 2>/dev/null || true
}
trap cleanup INT TERM EXIT HUP

# 1) Read stdin
cat > "$TMP_IN"

# 2) Terminal size (stdin may not be tty)
TERM_SIZE=$(stty size < /dev/tty 2>/dev/null || /bin/echo "24 80")
TERM_LINES=${TERM_SIZE% *}
TERM_COLS=${TERM_SIZE#* }

# 3) Input text dimensions (for centering)
# NOTE: awk length() counts characters in the current locale; keep as-is for typical UTF-8 locales.
TEXT_DIM=$(awk '{ if (length($0) > max) max = length($0) } END { print NR, max+0 }' "$TMP_IN")
TEXT_LINES=${TEXT_DIM% *}
TEXT_COLS=${TEXT_DIM#* }

OFFSET_Y=$(( (TERM_LINES - TEXT_LINES) / 2 ))
OFFSET_X=$(( (TERM_COLS - TEXT_COLS) / 2 ))
[ "$OFFSET_Y" -lt 0 ] && OFFSET_Y=0
[ "$OFFSET_X" -lt 0 ] && OFFSET_X=0

# 4) Convert to UTF-16BE (supports non-BMP via surrogate pairs)
# If input isn't UTF-8, fail clearly.
if ! iconv -f UTF-8 -t UTF-16BE "$TMP_IN" > "$TMP_U16" 2>/dev/null; then
  /bin/echo "Error: iconv failed. Ensure input is UTF-8." >&2
  exit 1
fi

# 5) Build coordinate list efficiently and robustly.
# Use od to output 2 bytes at a time: each line gives two hex bytes.
# Example line: " 00 41"
# We'll post-process without echo|tr per-iteration.
od -An -v -t x1 -w2 "$TMP_U16" > "$TMP_OD"

CUR_Y=0
CUR_X=0

# One-pass generation of list using single redirection (no >> in loop)
: > "$TMP_LIST"
while IFS= read -r line; do
  # normalize spaces and split into two hex bytes
  # line may contain leading spaces; set -- will split on IFS
  set -- $line
  b1=$1
  b2=$2

  # If od produced an empty line (shouldn't), skip
  [ -n "${b2:-}" ] || continue

  # Build 16-bit unit hex (uppercase/lowercase irrelevant)
  hex="${b1}${b2}"

  # Newline in UTF-16BE: 00 0a
  if [ "$hex" = "000a" ]; then
    CUR_Y=$((CUR_Y + 1))
    CUR_X=0
    continue
  fi

  # Ignore CR: 00 0d
  if [ "$hex" = "000d" ]; then
    continue
  fi

  # Surrogate handling (UTF-16)
  # High surrogate: D800-DBFF ; Low surrogate: DC00-DFFF
  # We'll detect by first byte:
  #   high surrogate first byte is d8..db
  #   low  surrogate first byte is dc..df
  b1_lc=$(printf '%s' "$b1" | tr 'A-F' 'a-f')

  if printf '%s\n' "$b1_lc" | grep -Eq '^(d8|d9|da|db)$'; then
    # Need to read the next 16-bit unit as low surrogate
    if ! IFS= read -r line2; then
      break
    fi
    set -- $line2
    c1=$1
    c2=$2
    [ -n "${c2:-}" ] || break

    c1_lc=$(printf '%s' "$c1" | tr 'A-F' 'a-f')
    if printf '%s\n' "$c1_lc" | grep -Eq '^(dc|dd|de|df)$'; then
      # Record 4-byte UTF-16BE sequence as one "character" cell
      /bin/echo "$((CUR_Y + OFFSET_Y)) $((CUR_X + OFFSET_X)) $b1 $b2 $c1 $c2" >> "$TMP_LIST"
      CUR_X=$((CUR_X + 1))
      continue
    else
      # Not a valid surrogate pair; fall back: output high surrogate as-is
      /bin/echo "$((CUR_Y + OFFSET_Y)) $((CUR_X + OFFSET_X)) $b1 $b2" >> "$TMP_LIST"
      CUR_X=$((CUR_X + 1))
      # The next unit (line2) is not consumed as content; but we already consumed it.
      # Best effort: treat it as a normal unit by handling it immediately:
      b1=$c1
      b2=$c2
      hex="${b1}${b2}"
      if [ "$hex" = "000a" ]; then
        CUR_Y=$((CUR_Y + 1))
        CUR_X=0
        continue
      fi
      if [ "$hex" = "000d" ]; then
        continue
      fi
      /bin/echo "$((CUR_Y + OFFSET_Y)) $((CUR_X + OFFSET_X)) $b1 $b2" >> "$TMP_LIST"
      CUR_X=$((CUR_X + 1))
      continue
    fi
  fi

  # Normal BMP unit: record 2 bytes
  /bin/echo "$((CUR_Y + OFFSET_Y)) $((CUR_X + OFFSET_X)) $b1 $b2" >> "$TMP_LIST"
  CUR_X=$((CUR_X + 1))
done < "$TMP_OD"

# 6) Shuffle
if [ -s "$TMP_LIST" ]; then
  shuf "$TMP_LIST" > "$TMP_SHUF"
else
  : > "$TMP_SHUF"
fi

# 7) Draw
tput clear 2>/dev/null || clear
tput civis 2>/dev/null || true

# Use external printf if desired; POSIX printf is typically external or builtin, either is fine.
# We'll just use "printf" directly.

# Each line: y x b1 b2 [c1 c2]
# Build bytes then iconv back to UTF-8 for terminal output.
while IFS= read -r ln; do
  set -- $ln
  y=$1
  x=$2
  b1=$3
  b2=$4
  c1=${5:-}
  c2=${6:-}

  tput cup "$y" "$x" 2>/dev/null || true

  if [ -n "$c2" ]; then
    # 4 bytes
    # shellcheck disable=SC2059
    printf "\\x$b1\\x$b2\\x$c1\\x$c2" | iconv -f UTF-16BE -t UTF-8
  else
    # 2 bytes
    # shellcheck disable=SC2059
    printf "\\x$b1\\x$b2" | iconv -f UTF-16BE -t UTF-8
  fi

  [ "$DELAY" != "0" ] && sleep "$DELAY"
done < "$TMP_SHUF"

# Move cursor to last line, show cursor
tput cup "$((TERM_LINES - 1))" 0 2>/dev/null || true
tput cnorm 2>/dev/null || true


