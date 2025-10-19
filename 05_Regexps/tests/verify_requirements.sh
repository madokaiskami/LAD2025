#!/usr/bin/env bash
set -euo pipefail
pass() { printf "\e[32m[PASS]\e[0m %s\n" "$*"; }
fail() { printf "\e[31m[FAIL]\e[0m %s\n" "$*"; exit 1; }

cmp() {
  local r="$1" s="$2" str="$3"
  local a b
  a="$(./esub "$r" "$s" "$str")"
  b="$(printf '%s\n' "$str" | sed -E "s/${r//\//\\/}/${s//\//\\/}/")"
  [[ "$a" == "$b" ]] || { echo "esub=[$a] sed=[$b]"; return 1; }
}

cmp '[0-9]' '#' 'a1b2c3' && pass "single substitution"
cmp 'a+' 'X' 'caaab' && pass "ERE quantifier +"
cmp '([[:digit:]]+)' '<\1>' 'ab123cd' && pass "ERE grouping and backref"

./esub '([0-9]+)' 'X\1Y' 'abc123def' | grep -qx 'abcX123Ydef' && pass "\1 basic"
./esub '(1)' '\11' '1' | grep -qx '11' && pass "\11 == \1 + 1"
./esub '^([a])?(b)$' 'X\1Y' 'b' | grep -qx 'XYb' && pass "missing group -> empty"

if ./esub '([a])' '\9' 'a' >/dev/null 2>&1; then fail "nonexistent group must error"; else pass "nonexistent group errors"; fi
if ./esub '([a-z]+' 'X' 'abc' >/dev/null 2>&1; then fail "bad regex must error"; else pass "regerror diagnostics"; fi

echo OK
