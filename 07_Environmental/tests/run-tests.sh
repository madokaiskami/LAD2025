#!/usr/bin/env bash
set -euo pipefail

# --- locate built binary robustly (in-tree / out-of-tree) ---
if [[ -n "${abs_top_builddir:-}" ]]; then
  prog="${abs_top_builddir}/src/rhasher"
elif [[ -n "${top_builddir:-}" ]]; then
  prog="${top_builddir}/src/rhasher"
else
  # compute from script location -> project root -> src/rhasher
  script_dir="$(cd "$(dirname "$0")" && pwd)"
  root_dir="$(cd "${script_dir}/.." && pwd)"
  prog="${root_dir}/src/rhasher"
  [[ -x "$prog" ]] || prog="../src/rhasher"
fi

if [[ ! -x "$prog" ]]; then
  echo "FATAL: cannot find built binary: $prog" >&2
  exit 2
fi

# --- test data location ---
: "${srcdir:=$(dirname "$0")}"

# 1) File-based SHA1/MD5 (uppercase hex)
LC_ALL=C sha1_ref=$(sha1sum "${srcdir}/data/hello.txt" | awk '{print toupper($1)}')
LC_ALL=C md5_ref=$(md5sum  "${srcdir}/data/hello.txt" | awk '{print toupper($1)}')

out="$(
  printf 'SHA1 %s\nMD5 %s\n' "${srcdir}/data/hello.txt" "${srcdir}/data/hello.txt" | "${prog}"
)"

sha1_out=$(printf "%s" "$out" | sed -n '1p')
md5_out=$(printf "%s" "$out" | sed -n '2p')

if [[ "$sha1_out" != "$sha1_ref" ]]; then
  echo "FAIL: SHA1 mismatch" >&2
  echo " got: $sha1_out" >&2
  echo " ref: $sha1_ref" >&2
  exit 1
fi

if [[ "$md5_out" != "$md5_ref" ]]; then
  echo "FAIL: MD5 mismatch" >&2
  echo " got: $md5_out" >&2
  echo " ref: $md5_ref" >&2
  exit 1
fi

# 2) String-based SHA1 hex and md5 base64 sanity
LC_ALL=C sha1_str_ref=$(printf "abc" | sha1sum | awk '{print toupper($1)}')

out2="$(
  printf 'SHA1 \"abc\"\nmd5 \"hello\"\n' | "${prog}"
)"
sha1_str_out=$(printf "%s" "$out2" | sed -n '1p')
md5_b64_out=$(printf "%s" "$out2" | sed -n '2p')

if [[ "$sha1_str_out" != "$sha1_str_ref" ]]; then
  echo "FAIL: SHA1 string mismatch" >&2
  echo " got: $sha1_str_out" >&2
  echo " ref: $sha1_str_ref" >&2
  exit 1
fi

# Base64 smoke check: non-empty and plausible length (>=22 for MD5)
if [[ -z "$md5_b64_out" || ${#md5_b64_out} -lt 22 ]]; then
  echo "FAIL: md5 base64 looks wrong: '$md5_b64_out'" >&2
  exit 1
fi

echo "OK"
