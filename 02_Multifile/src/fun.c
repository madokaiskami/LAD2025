#include "output.h"
#include <stdio.h>

int output_main(const char *progname, int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <arg1> [arg2] [arg3]\n", progname);
        return 2;
    }

    printf("Program: %s\n", progname);
    printf("Argc=%d\n", argc);
    for (int i = 0; i < argc; ++i) {
        const char *val;
        if (i == 0) {
            val = progname; // normalize argv[0]
        } else {
            val = (argv[i] ? argv[i] : "(null)");
        }
        printf("argv[%d]=%s\n", i, val);
    }

    extern const char *OUTPUT_TAG;
    printf("Tag=%s\n", OUTPUT_TAG);
    return 0;
}
