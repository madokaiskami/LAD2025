
#define _GNU_SOURCE
#include <errno.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/* 
 * LD_PRELOAD-able library that prevents deletion of any path whose
 * name contains the substring "PROTECT". It intercepts unlink, unlinkat,
 * and remove. For non-matching paths, it calls the real functions.
 */

static int contains_protect(const char *path) {
    if (!path) return 0;
    return strstr(path, "PROTECT") != NULL;
}

typedef int (*unlink_fn)(const char *);
typedef int (*unlinkat_fn)(int, const char *, int);
typedef int (*remove_fn)(const char *);

static void *sym(const char *name) {
    void *p = dlsym(RTLD_NEXT, name);
    if (!p) {
        fprintf(stderr, "protect.so: failed to resolve %s: %s\n", name, dlerror());
    }
    return p;
}

int unlink(const char *pathname) {
    if (contains_protect(pathname)) {
        /* Deny deletion */
        errno = EPERM;
        fprintf(stderr, "protect.so: refusing to unlink '%s'\n", pathname);
        return -1;
    }
    unlink_fn real_unlink = (unlink_fn)sym("unlink");
    return real_unlink(pathname);
}

int unlinkat(int dirfd, const char *pathname, int flags) {
    if (contains_protect(pathname)) {
        errno = EPERM;
        fprintf(stderr, "protect.so: refusing to unlinkat '%s'\n", pathname);
        return -1;
    }
    unlinkat_fn real_unlinkat = (unlinkat_fn)sym("unlinkat");
    return real_unlinkat(dirfd, pathname, flags);
}

int remove(const char *pathname) {
    if (contains_protect(pathname)) {
        errno = EPERM;
        fprintf(stderr, "protect.so: refusing to remove '%s'\n", pathname);
        return -1;
    }
    remove_fn real_remove = (remove_fn)sym("remove");
    return real_remove(pathname);
}
