#ifndef GROWABLE_BUF_H
#define GROWABLE_BUF_H

/*
 * Growable memory buffer for C99.
 *
 * Public interface (all macros), matching the semantics described in
 * "Growable Memory Buffers for C99":
 *
 *   size_t buf_size(type *v);
 *   size_t buf_capacity(type *v);
 *   void   buf_free(type *v);
 *   void   buf_push(type *v, type e);
 *   type   buf_pop(type *v);
 *   type  *buf_grow(type *v, ptrdiff_t n);
 *   type  *buf_trunc(type *v, ptrdiff_t n);
 *   void   buf_clear(type *v);
 *
 * All of these are implemented as macros here.  The only real function is
 * `buf__grow_raw` in buf.c, which performs allocation/reallocation and
 * capacity management.  The pointer `v` may change after push/grow/trunc/
 * free, so callers must always use the updated value.
 */

#include <stddef.h>   /* size_t, ptrdiff_t, offsetof */
#include <stdlib.h>   /* abort, malloc, realloc, free */
#include <stdint.h>

#ifndef BUF_INIT_CAPACITY
# define BUF_INIT_CAPACITY 8u
#endif

#ifndef BUF_ABORT
# define BUF_ABORT abort()
#endif

struct buf_hdr {
    size_t size;     /* number of elements actually stored */
    size_t capacity; /* number of elements allocated */
    unsigned char buf[]; /* flexible array member (payload) */
};

/* Internal helper: from user pointer back to header. */
#define BUF__HDR(v) \
    ((struct buf_hdr *)((unsigned char *)(v) - offsetof(struct buf_hdr, buf)))

/* Internal function implemented in buf.c.
 *
 * Ensures that the buffer has at least `new_capacity` elements of size
 * `elem_size`. Returns the (possibly moved) data pointer, not the header.
 * On allocation failure or overflow it evaluates BUF_ABORT.
 */
void *buf__grow_raw(void *buf, size_t new_capacity, size_t elem_size);

/* Public macros */

/* Number of elements currently stored. */
#define buf_size(v) ((v) ? BUF__HDR(v)->size : (size_t)0)

/* Capacity in number of elements. */
#define buf_capacity(v) ((v) ? BUF__HDR(v)->capacity : (size_t)0)

/* Free storage and reset pointer to NULL. */
#define buf_free(v)                                  \
    do {                                             \
        if (v) {                                     \
            free(BUF__HDR(v));                       \
            (v) = NULL;                              \
        }                                            \
    } while (0)

/* Append a single element, growing if needed. */
#define buf_push(v, value)                                           \
    do {                                                             \
        size_t __buf_sz = buf_size(v);                               \
        size_t __buf_cap = buf_capacity(v);                          \
        if (__buf_sz >= __buf_cap) {                                 \
            (v) = buf__grow_raw((v), __buf_sz + 1u, sizeof(*(v)));   \
        }                                                            \
        BUF__HDR(v)->size = __buf_sz + 1u;                           \
        (v)[__buf_sz] = (value);                                     \
    } while (0)

/* Pop last element and return it.  Undefined behaviour if size == 0. */
#define buf_pop(v)                                                   \
    (                                                                 \
        BUF__HDR(v)->size--,                                         \
        (v)[BUF__HDR(v)->size]                                       \
    )

/* Increase capacity by N elements, return updated pointer. */
#define buf_grow(v, n)                                               \
    (                                                                 \
        (v) = buf__grow_raw((v), buf_capacity(v) + (size_t)(n),      \
                            sizeof(*(v)) )                           \
    )

/* Set capacity to exactly N elements, adjust size if needed. */
#define buf_trunc(v, n)                                              \
    (                                                                 \
        (v) = buf__grow_raw((v), (size_t)(n), sizeof(*(v))),         \
        BUF__HDR(v)->size =                                          \
            buf_size(v) > (size_t)(n) ? (size_t)(n) : buf_size(v),   \
        (v)                                                          \
    )

/* Logical clear: keep capacity/data, set size to zero. */
#define buf_clear(v)                                                 \
    do {                                                             \
        if (v) {                                                     \
            BUF__HDR(v)->size = 0u;                                  \
        }                                                            \
    } while (0)

#endif /* GROWABLE_BUF_H */
