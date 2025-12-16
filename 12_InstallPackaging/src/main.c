#include "greeter.h"

#include <getopt.h>
#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

static void print_help(const char *prog) {
    printf(greeter_translate("Usage: %s [--name NAME]\n"), prog);
    fputs(greeter_translate("Options:\n"), stdout);
    fputs(greeter_translate("  --name, -n   Name to greet\n"), stdout);
    fputs(greeter_translate("  --help, -h   Show this help message\n"), stdout);
}

int main(int argc, char *argv[]) {
    greeter_init_locale();

    static struct option long_opts[] = {
        {"name", required_argument, NULL, 'n'},
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}};

    const char *name = NULL;
    int opt;
    while ((opt = getopt_long(argc, argv, "n:h", long_opts, NULL)) != -1) {
        switch (opt) {
        case 'n':
            name = optarg;
            break;
        case 'h':
            print_help(argv[0]);
            return EXIT_SUCCESS;
        default:
            print_help(argv[0]);
            return EXIT_FAILURE;
        }
    }

    char buffer[256];
    if (greeter_build_message(name, buffer, sizeof(buffer)) != 0) {
        fprintf(stderr, "%s", greeter_translate("Name too long.\n"));
        return EXIT_FAILURE;
    }
    printf("%s\n", buffer);
    return EXIT_SUCCESS;
}
