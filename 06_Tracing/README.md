
# 06_Tracing

Course HW: tracing (strace/ltrace), errno, ld.so, LD_PRELOAD.

## Build

```bash
make            # builds `move` and `libprotect.so`
```

## Usage

```bash
./move infile outfile
# copies bytes from infile to outfile (truncating/creating outfile),
# then deletes infile on success.
# Distinct non-zero exit codes indicate specific failure reasons.
```

## Tests

Requires `strace` with fault injection support.

```bash
make test
```

The test suite uses **strace error injection** to simulate failures of `openat`, `read`, `write`, `fsync`, and `close`, and verifies:
- program exit codes
- which file remains (safety guarantee)

It also tests `LD_PRELOAD` with `libprotect.so` that prevents deleting files whose name contains `PROTECT`.

## Exit codes

| Code | Meaning |
|-----:|--------|
| 0 | success |
| 64 | usage error |
| 65 | `stat(infile)` failed |
| 66 | open infile failed |
| 67 | open/create outfile failed |
| 68 | infile and outfile are the **same** object |
| 69 | read failed |
| 70 | write failed |
| 71 | fsync failed |
| 72 | close(outfile) failed |
| 73 | close(infile) failed |
| 74 | unlink(infile) failed |
| 75 | out-of-memory |

## Notes

- Implementation copies data (no `link(2)`), optionally reading entire file into memory (up to 256 MiB), otherwise streaming in 1 MiB chunks.
- The program never deletes the source until the target is fully written, `fsync`'ed, and closed; on any failure, it removes the partial target and keeps the source.
- The preload library overrides `unlink`, `unlinkat`, and `remove` and denies operations when the path contains `PROTECT` (returns `EPERM`).

