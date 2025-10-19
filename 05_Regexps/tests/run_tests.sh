#!/usr/bin/env bash
set -euo pipefail

echo '[TEST] compare against sed -E ...'
fail=0

while IFS=$'\t' read -r re sub s; do
  [[ -z "$re" ]] && continue
  out_esub="$(./esub "$re" "$sub" "$s")"
  out_sed="$(./tests/sed_once.sh "$re" "$sub" "$s")"
  if [[ "$out_esub" == "$out_sed" ]]; then
    printf '  [OK]  %s | %s | %s\n' "$re" "$sub" "$s"
  else
    printf '  [FAIL] %s | %s | %s\n' "$re" "$sub" "$s"
    printf '    esub: %s\n' "$out_esub"
    printf '    sed : %s\n' "$out_sed"
    fail=$((fail+1))
  fi
done < tests/cases.tsv

echo
echo '[TEST] error cases (expect non-zero exit) ...'
while IFS=$'\t' read -r re sub s; do
  [[ -z "$re" ]] && continue
  if ./esub "$re" "$sub" "$s" >/dev/null 2>tests/err.txt; then
    printf '  [FAIL] %s | %s | %s (exit=0)\n' "$re" "$sub" "$s"
    fail=$((fail+1))
  else
    printf '  [OK]   %s | %s | %s\n' "$re" "$sub" "$s"
  fi
done < tests/error_cases.tsv

exit $fail
