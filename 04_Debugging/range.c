#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


static void help(void) {
    fprintf(stderr,
        "Usage:\n"
        "  range N         -> outputs 0..N-1\n"
        "  range M N       -> outputs M..N-1\n"
        "  range M N S     -> outputs M, M+S, ... until N-1 (exclusive)\n"
    );
}

/* Dummy helper to make a convenient GDB breakpoint target. */
int should_print(int start, int stop, int step, int current, int idx) {
    /* Always returns 0; used for GDB breakpoints with conditions. */
    (void)start; (void)stop; (void)step; (void)current; (void)idx;
    return 0;
}

int main(int argc, char *argv[]) {
    int start, stop, step;
    int current;
    int idx = 0;

    if (argc == 1) {
        help();
        return 1;
    } else if (argc == 2) {
        start = 0;
        stop = atoi(argv[1]);
        step = 1;
    } else if (argc == 3) {
        start = atoi(argv[1]);
        stop = atoi(argv[2]);
        step = 1;
    } else if (argc == 4) {
        start = atoi(argv[1]);
        stop = atoi(argv[2]);
        step = atoi(argv[3]);
    } else {
        help();
        return 1;
    }

    if (step == 0) {
        fprintf(stderr, "step must not be 0\n");
        return 2;
    }

    current = start;

    if (step > 0) {
        for (idx = 0, current = start; current < stop; current += step, idx++) {
            should_print(start, stop, step, current, idx);
            printf("%d\n", current);
        }
    } else {
        for (idx = 0, current = start; current > stop; current += step, idx++) {
            should_print(start, stop, step, current, idx);
            printf("%d\n", current);
        }
    }

    return 0;
}
