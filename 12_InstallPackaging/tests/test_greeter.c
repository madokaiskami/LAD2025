#include "greeter.h"

#include <locale.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    setlocale(LC_ALL, "C");
    greeter_init_locale();

    char buffer[64];
    if (greeter_build_message("Unit", buffer, sizeof(buffer)) != 0) {
        fprintf(stderr, "Failed to build greeting\n");
        return 1;
    }
    if (strstr(buffer, "Unit") == NULL) {
        fprintf(stderr, "Greeting missing name: %s\n", buffer);
        return 1;
    }

    if (greeter_build_message(NULL, buffer, 4) == 0) {
        fprintf(stderr, "Expected buffer overflow\n");
        return 1;
    }

    return 0;
}
