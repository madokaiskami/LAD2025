#include "buf.h"

#include <limits.h>

/* Internal helper: safe multiplication with overflow check.
 * Returns 0 on success, non-zero on overflow.
 */
static int mul_overflow(size_t a, size_t b, size_t *result)
{
    if (a == 0 || b == 0) {
        *result = 0;
        return 0;
    }
    if (a > SIZE_MAX / b) {
        return 1;
    }
    *result = a * b;
    return 0;
}

/* Grow/shrink buffer to at least new_capacity elements.
 *
 * Layout in memory:
 *   [ struct buf_hdr { size, capacity; unsigned char buf[]; } ][ payload... ]
 *
 * buf (user pointer) == &hdr->buf[0]
 */
void *buf__grow_raw(void *buf, size_t new_capacity, size_t elem_size)
{
    struct buf_hdr *hdr = NULL;
    size_t capacity = 0;

    if (buf) {
        hdr = BUF__HDR(buf);
        capacity = hdr->capacity;
    }

    /* Nothing to do if already large enough. */
    if (new_capacity <= capacity) {
        return buf ? buf : NULL;
    }

    /* At least BUF_INIT_CAPACITY and grow exponentially. */
    if (capacity < BUF_INIT_CAPACITY) {
        capacity = BUF_INIT_CAPACITY;
    }
    while (capacity < new_capacity) {
        size_t next = capacity ? capacity * 2u : BUF_INIT_CAPACITY;
        if (next < capacity) {
            /* overflow */
            BUF_ABORT;
        }
        capacity = next;
    }

    /* Compute total bytes with overflow check. */
    size_t payload_bytes;
    if (mul_overflow(capacity, elem_size, &payload_bytes)) {
        BUF_ABORT;
    }

    size_t total_bytes;
    if (mul_overflow(sizeof(struct buf_hdr), 1u, &total_bytes)) {
        BUF_ABORT;
    }
    if (SIZE_MAX - total_bytes < payload_bytes) {
        BUF_ABORT;
    }
    total_bytes += payload_bytes;

    if (hdr) {
        hdr = (struct buf_hdr *)realloc(hdr, total_bytes);
    } else {
        hdr = (struct buf_hdr *)malloc(total_bytes);
        if (hdr) {
            hdr->size = 0u;
        }
    }

    if (!hdr) {
        BUF_ABORT;
    }

    hdr->capacity = capacity;
    return hdr->buf;
}
