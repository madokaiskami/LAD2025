#!/usr/bin/env bash
set -euo pipefail

# Colors
ok() { printf "\033[32m[OK]\033[0m %s\n" "$*"; }
fail() { printf "\033[31m[FAIL]\033[0m %s\n" "$*"; exit 1; }

# Build first (expects Makefile in the same dir)
make -s

# Exit codes (must match move.c)
EX_OK=0
EX_USAGE=64
EX_STAT_IN=65
EX_OPEN_IN=66
EX_OPEN_OUT=67
EX_SAME_FILE=68
EX_READ=69
EX_WRITE=70
EX_FSYNC=71
EX_CLOSE_OUT=72
EX_CLOSE_IN=73
EX_UNLINK_IN=74
EX_MEMORY=75

TMPDIR="$(mktemp -d)"
cleanup() { rm -rf "$TMPDIR"; }
trap cleanup EXIT

IN="$TMPDIR/in.txt"
OUT="$TMPDIR/out.txt"

# helper: create IN with content
echo "hello world" > "$IN"

# 1) happy path
./move "$IN" "$OUT"
code=$?
[[ $code -eq $EX_OK ]] || fail "happy path exit code $code != $EX_OK"
[[ ! -e "$IN" ]] || fail "infile should be removed"
diff -u <(echo "hello world") "$OUT" >/dev/null || fail "content mismatch"
ok "happy path"

# Recreate IN for error-injection tests
echo "hello world" > "$IN"

which strace >/dev/null 2>&1 || { echo "strace not found; skipping injection tests"; exit 0; }

# 2) OPEN OUT failure via real permission (robust across strace versions)
#    Create a read-only directory so open(O_CREAT|O_TRUNC) fails with EACCES.
echo "hello world" > "$IN"
RODIR="$TMPDIR/rodir"
mkdir -p "$RODIR"
chmod 0555 "$RODIR"
OUT="$RODIR/out.txt"
set +e
./move "$IN" "$OUT" >/dev/null 2>&1
code=$?
set -e
chmod 0755 "$RODIR"
[[ $code -eq $EX_OPEN_OUT ]] || fail "permission test: exit $code, expected $EX_OPEN_OUT"
[[ -e "$IN" ]] || fail "infile must remain after open(out) permission failure"
[[ ! -e "$OUT" ]] || fail "outfile must not exist after open(out) permission failure"
OUT="$TMPDIR/out.txt"
ok "permission-based open(out) failure"

# 3) inject write failure (EIO) on OUT
echo "hello world" > "$IN"
set +e
strace -qq -P "$OUT" -e fault=write:error=EIO:when=1 ./move "$IN" "$OUT" >/dev/null 2>&1
code=$?
set -e
[[ $code -eq $EX_WRITE ]] || fail "write injection: exit $code, expected $EX_WRITE"
[[ -e "$IN" ]] || fail "infile must remain after write failure"
[[ ! -e "$OUT" ]] || fail "outfile must be removed after write failure"
ok "inject write -> EIO on OUT"

# 4) inject fsync failure on OUT
echo "hello world" > "$IN"
set +e
strace -qq -P "$OUT" -e fault=fsync:error=EIO:when=1 ./move "$IN" "$OUT" >/dev/null 2>&1
code=$?
set -e
[[ $code -eq $EX_FSYNC ]] || fail "fsync injection: exit $code, expected $EX_FSYNC"
[[ -e "$IN" ]] || fail "infile must remain after fsync failure"
[[ ! -e "$OUT" ]] || fail "outfile must be removed after fsync failure"
ok "inject fsync -> EIO on OUT"

# 5) inject close failure on OUT
echo "hello world" > "$IN"
set +e
strace -qq -P "$OUT" -e fault=close:error=EIO:when=1 ./move "$IN" "$OUT" >/dev/null 2>&1
code=$?
set -e
[[ $code -eq $EX_CLOSE_OUT ]] || fail "close(out) injection: exit $code, expected $EX_CLOSE_OUT"
[[ -e "$IN" ]] || fail "infile must remain after close(out) failure"
[[ ! -e "$OUT" ]] || fail "outfile must be removed after close(out) failure"
ok "inject close -> EIO on OUT"

# 6) inject read failure on IN
echo "hello world" > "$IN"
set +e
strace -qq -P "$IN" -e fault=read:error=EIO:when=1 ./move "$IN" "$OUT" >/dev/null 2>&1
code=$?
set -e
[[ $code -eq $EX_READ ]] || fail "read injection: exit $code, expected $EX_READ"
[[ -e "$IN" ]] || fail "infile must remain after read failure"
[[ ! -e "$OUT" ]] || fail "outfile must be removed after read failure"
ok "inject read -> EIO on IN"

# 7) LD_PRELOAD protection: infile containing PROTECT must not be deleted
echo "secret" > "$TMPDIR/PROTECT_file.txt"
set +e
LD_PRELOAD="$(pwd)/libprotect.so" ./move "$TMPDIR/PROTECT_file.txt" "$TMPDIR/protected_out.txt" >/dev/null 2>&1
code=$?
set -e
[[ $code -eq $EX_UNLINK_IN ]] || fail "LD_PRELOAD test: exit $code, expected $EX_UNLINK_IN (unlink denied)"
[[ -e "$TMPDIR/PROTECT_file.txt" ]] || fail "LD_PRELOAD test: infile should not be deleted"
diff -u <(echo "secret") "$TMPDIR/protected_out.txt" >/dev/null || fail "LD_PRELOAD test: content mismatch"
ok "LD_PRELOAD test (PROTECT blocks unlink)"
echo "All tests passed."
