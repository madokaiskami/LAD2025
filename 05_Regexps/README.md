# 05_Regexps — `esub`

This project implements the homework from **LinuxApplicationDevelopment2025 / 05_Regexps**.

## Build

```sh
make
```

## Usage

```sh
./esub REGEXP SUBSTITUTION STRING
```

Behaves like:

```sh
echo "STRING" | sed -E 's/REGEXP/SUBSTITUTION/'
```

- Uses **extended regular expressions** (POSIX ERE).
- Prints the original string unchanged if there is no match.
- In `SUBSTITUTION`:
  - `\1` … `\9` insert capture groups 1–9 (at most 100 occurrences total).
  - `\\` inserts a single backslash `\`.
  - A reference to a non-existent group is an **error**.
  - `\11` is treated as `\1` followed by literal `1` (classic syntax).

## Tests

Compare `esub` to `sed -E` for a set of cases and also check error handling:

```sh
make test
```

## Notes

- Regex diagnostics are reported using `regerror()`.
- Only **single** substitution is performed (no `g` flag).
- Group 0 (the whole match) is not addressable (like `sed`; use `&` in sed if needed).

