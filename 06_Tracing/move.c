
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * move infile outfile
 * - copy bytes from infile to outfile (truncating/creating outfile)
 * - on success, delete infile
 * Safety requirements:
 *  - never delete infile until outfile is fully written, fsync'ed and closed
 *  - if something goes wrong after creating outfile, remove (unlink) outfile
 *  - check and report as many errors as possible, set distinct exit codes
 *  - do NOT use link(2)
 */

enum ExitCode {
    EX_OK = 0,
    EX_USAGE = 64,
    EX_STAT_IN = 65,
    EX_OPEN_IN = 66,
    EX_OPEN_OUT = 67,
    EX_SAME_FILE = 68,
    EX_READ = 69,
    EX_WRITE = 70,
    EX_FSYNC = 71,
    EX_CLOSE_OUT = 72,
    EX_CLOSE_IN = 73,
    EX_UNLINK_IN = 74,
    EX_MEMORY = 75
};

static void perrorf(const char *ctx, const char *path) {
    if (path) {
        fprintf(stderr, "%s: %s: %s\n", ctx, path, strerror(errno));
    } else {
        fprintf(stderr, "%s: %s\n", ctx, strerror(errno));
    }
}

static int safe_unlink(const char *path) {
    if (unlink(path) == -1) {
        perrorf("unlink", path);
        return -1;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s infile outfile\n", argv[0]);
        return EX_USAGE;
    }
    const char *inpath = argv[1];
    const char *outpath = argv[2];

    /* stat() both to catch some edge cases and gather permissions/size */
    struct stat inst, outst;
    if (stat(inpath, &inst) == -1) {
        perrorf("stat(infile)", inpath);
        return (errno == ENOENT) ? EX_STAT_IN : EX_STAT_IN;
    }
    if (S_ISDIR(inst.st_mode)) {
        fprintf(stderr, "infile is a directory: %s\n", inpath);
        return EX_STAT_IN;
    }
    if (stat(outpath, &outst) == 0) {
if (S_ISDIR(outst.st_mode)) {
            fprintf(stderr, "outfile is a directory: %s\n", outpath);
            return EX_OPEN_OUT;
        }
        /* If infile and outfile are the same file (same device+inode), bail */
        if (inst.st_dev == outst.st_dev && inst.st_ino == outst.st_ino) {
            fprintf(stderr, "infile and outfile refer to the same file\n");
            return EX_SAME_FILE;
        }
    }

    /* open infile */
    int in_fd = open(inpath, O_RDONLY | O_CLOEXEC);
    if (in_fd == -1) {
        perrorf("open(infile)", inpath);
        return EX_OPEN_IN;
    }

    /* open/create outfile with 0666 masked by umask, truncate existing */
    mode_t mode = 0666;
    int out_fd = open(outpath, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, mode);
    if (out_fd == -1) {
        perrorf("open(outfile)", outpath);
        close(in_fd); /* ignore close error on the way out */
        return EX_OPEN_OUT;
    }

    /* Decide copy strategy: whole-file into memory if size sane, else chunked */
    size_t buf_sz = 0;
    int use_whole = 0;
    if (S_ISREG(inst.st_mode) && inst.st_size >= 0 && inst.st_size <= (off_t)(256 * 1024 * 1024)) {
        /* up to 256 MiB read into memory is allowed by the task */
        buf_sz = (size_t)inst.st_size;
        use_whole = 1;
    } else {
        buf_sz = 1024 * 1024; /* 1 MiB chunks */
    }

    int exitcode = EX_OK;
    char *buf = NULL;
    if (use_whole) {
        buf = (char*)malloc(buf_sz ? buf_sz : 1);
        if (!buf) {
            perrorf("malloc", NULL);
            exitcode = EX_MEMORY;
            goto CLEANUP_ON_FAILURE;
        }
        /* read exactly file size */
        size_t total = 0;
        while (total < buf_sz) {
            ssize_t r = read(in_fd, buf + total, buf_sz - total);
            if (r < 0) {
                perrorf("read", inpath);
                exitcode = EX_READ;
                goto CLEANUP_ON_FAILURE;
            }
            if (r == 0) break;
            total += (size_t)r;
        }
        if (total != buf_sz) {
            /* infile was truncated/grown during read; handle by using what we read */
            buf_sz = total;
        }
        /* write all */
        size_t wtot = 0;
        while (wtot < buf_sz) {
            ssize_t w = write(out_fd, buf + wtot, buf_sz - wtot);
            if (w < 0) {
                perrorf("write", outpath);
                exitcode = EX_WRITE;
                goto CLEANUP_ON_FAILURE;
            }
            wtot += (size_t)w;
        }
    } else {
        buf_sz = 1024 * 1024;
        buf = (char*)malloc(buf_sz);
        if (!buf) {
            perrorf("malloc", NULL);
            exitcode = EX_MEMORY;
            goto CLEANUP_ON_FAILURE;
        }
        while (1) {
            ssize_t r = read(in_fd, buf, buf_sz);
            if (r < 0) {
                perrorf("read", inpath);
                exitcode = EX_READ;
                goto CLEANUP_ON_FAILURE;
            }
            if (r == 0) break;
            ssize_t off = 0;
            while (off < r) {
                ssize_t w = write(out_fd, buf + off, r - off);
                if (w < 0) {
                    perrorf("write", outpath);
                    exitcode = EX_WRITE;
                    goto CLEANUP_ON_FAILURE;
                }
                off += w;
            }
        }
    }

    /* Flush to disk before we even try unlinking the source */
    if (fsync(out_fd) == -1) {
        perrorf("fsync", outpath);
        exitcode = EX_FSYNC;
        goto CLEANUP_ON_FAILURE;
    }
    if (close(out_fd) == -1) {
        perrorf("close(outfile)", outpath);
        exitcode = EX_CLOSE_OUT;
        out_fd = -1; /* still treat as closed to avoid double-close */
        goto CLEANUP_ON_FAILURE;
    }
    out_fd = -1; /* mark closed */

    /* Now it is safe to remove the source */
    if (safe_unlink(inpath) == -1) {
        exitcode = EX_UNLINK_IN;
        goto CLEANUP_ONLY_INFILE; /* do NOT remove outfile */
    }

    /* Finalize: close infile and exit OK */
    if (close(in_fd) == -1) {
        perrorf("close(infile)", inpath);
        /* We already deleted infile successfully; treat close(in) failure as separate */
        return EX_CLOSE_IN;
    }
    free(buf);
    return EX_OK;

CLEANUP_ON_FAILURE:
    /* Best-effort cleanup: do not touch infile, remove partial outfile */
    if (out_fd != -1) {
        /* try to ensure metadata/dirty page states are irrelevant */
        /* ignore fsync here; we're deleting anyway */
        close(out_fd);
        out_fd = -1;
    }
    /* remove partial target to be safe */
    if (unlink(outpath) == -1) {
        /* ignore error; nothing we can do */
    }
CLEANUP_ONLY_INFILE:
    /* Close infile last */
    if (in_fd != -1) {
        close(in_fd);
        in_fd = -1;
    }
    free(buf);
    return exitcode;
}
