#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <libintl.h>

#ifndef LOCALEDIR
#define LOCALEDIR "/usr/share/locale"
#endif

#define _(S) gettext(S)

static int read_line(char *buf, size_t sz) {
    if (fgets(buf, (int)sz, stdin) == NULL) {
        return -1; /* EOF or error */
    }
    size_t n = strlen(buf);
    if (n && buf[n-1] == '\n') buf[n-1] = '\0';
    return 0;
}

int main(void) {
    setlocale(LC_ALL, "");
    bindtextdomain("guessnum", LOCALEDIR);
    bind_textdomain_codeset("guessnum", "UTF-8");
    textdomain("guessnum");

    char buf[128];
    puts(_("Think of a number from 1 to 100, then press Enter."));
    fflush(stdout);
    if (read_line(buf, sizeof(buf)) < 0) {
        fputs(_("\nError: input closed (EOF). Aborting.\n"), stderr);
        return 1;
    }

    int lo = 1, hi = 100;
    const char *YES = _("Yes");
    const char *NO  = _("No");

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        printf(_("Is your number greater than %d? (\"%s\"/\"%s\"): "), mid, YES, NO);
        fflush(stdout);

        if (read_line(buf, sizeof(buf)) < 0) {
            fputs(_("\nError: input closed (EOF). Aborting.\n"), stderr);
            return 1;
        }
        if (strcmp(buf, YES) == 0) {
            lo = mid + 1;
        } else if (strcmp(buf, NO) == 0) {
            hi = mid;
        } else {
            fprintf(stderr, _("Invalid answer. Please type exactly \"%s\" or \"%s\".\n"), YES, NO);
            continue;
        }
    }

    printf(_("Guessed! Your number is %d.\n"), lo);
    return 0;
}
