#include "output.h"
#include <stdio.h>

int main(int argc, char **argv) {
    const char *progname = "prog";
    return output_main(progname, argc, argv);
}
