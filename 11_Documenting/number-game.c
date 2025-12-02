\
    /**
     * @file number-game.c
     * @brief Number guessing game with optional Roman numerals.
     *
     * This program implements a simple "guess the number" game. By default it
     * works with Arabic numerals, but with -r/--roman it uses Roman numerals
     * in the range 1..100.
     */

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <time.h>
    #include <getopt.h>

    #include "roman.h"

    /** @brief Minimum secret value (inclusive). */
    #define MIN_VALUE 1

    /** @brief Maximum secret value (inclusive). */
    #define MAX_VALUE 100

    /** @brief Program version string. */
    #define NUMBER_GAME_VERSION "0.1"

    /** @brief Global flag: use Roman numerals instead of Arabic. */
    static int g_use_roman = 0;

    /**
     * @brief Print command-line help (plain text) to the given stream.
     *
     * @param stream Output stream (stdout or stderr).
     * @param progname Program name to show in usage.
     */
    static void print_help(FILE *stream, const char *progname)
    {
        fprintf(stream,
                "Usage: %s [OPTIONS]\n"
                "\n"
                "Guess the number game / Игра «Угадай число»\n"
                "\n"
                "Options:\n"
                "  -r, --roman         Use Roman numerals (1..100) /\n"
                "                      Использовать римские числа (1..100)\n"
                "  -h, --help          Show this help and exit /\n"
                "                      Показать эту справку и выйти\n"
                "      --help-md       Show help in Markdown/Doxygen format /\n"
                "                      Показать справку в формате Markdown/Doxygen\n"
                "  -V, --version       Show program version and exit /\n"
                "                      Показать версию программы и выйти\n"
                "\n"
                "Description:\n"
                "  The program chooses a secret number between %d and %d and asks you to\n"
                "  guess it.\n"
                "  Программа загадывает число в диапазоне от %d до %d и предлагает\n"
                "  вам его угадать.\n",
                progname, MIN_VALUE, MAX_VALUE, MIN_VALUE, MAX_VALUE);
    }

    /**
     * @brief Print help in Markdown/Doxygen-friendly format.
     *
     * This output can be used to fill the Doxygen main page.
     *
     * @param stream Output stream (stdout).
     * @param progname Program name to show in usage.
     */
    static void print_help_md(FILE *stream, const char *progname)
    {
        fprintf(stream,
                "# number-game — Игра «Угадай число»\n"
                "\n"
                "## Usage / Использование\n"
                "\n"
                "`%s [OPTIONS]`\n"
                "\n"
                "## Options / Опции\n"
                "\n"
                "* `-r`, `--roman` — Use Roman numerals (1..100) / "
                "Использовать римские числа (1..100)\n"
                "* `-h`, `--help` — Show this help and exit / "
                "Показать эту справку и выйти\n"
                "* `--help-md` — Show help in Markdown/Doxygen format / "
                "Показать справку в формате Markdown/Doxygen\n"
                "* `-V`, `--version` — Show program version and exit / "
                "Показать версию программы и выйти\n"
                "\n"
                "## Description / Описание\n"
                "\n"
                "The program chooses a secret number between %d and %d and asks you to "
                "guess it.\n"
                "Программа загадывает число в диапазоне от %d до %d и предлагает вам "
                "его угадать.\n",
                progname, MIN_VALUE, MAX_VALUE, MIN_VALUE, MAX_VALUE);
    }

    /**
     * @brief Run the guessing game.
     *
     * @return Exit code (0 on success).
     */
    static int run_game(void)
    {
        int secret;
        char buf[128];

        /* Initialize RNG. */
        srand((unsigned int)time(NULL));

        secret = MIN_VALUE + rand() % (MAX_VALUE - MIN_VALUE + 1);

        printf("Guess the number game / Игра «Угадай число»\n");

        if (!g_use_roman) {
            printf("I have chosen a number between %d and %d.\n",
                   MIN_VALUE, MAX_VALUE);
            printf("Try to guess it!\n");
        } else {
            const char *min_r = arabic_to_roman(MIN_VALUE);
            const char *max_r = arabic_to_roman(MAX_VALUE);
            printf("I have chosen a Roman number between %s and %s.\n",
                   min_r, max_r);
            printf("Try to guess it (input Roman numerals).\n");
        }

        for (;;) {
            int guess;
            char *nl;

            if (!g_use_roman) {
                printf("Your guess (Arabic) / Ваш ответ (арабские): ");
            } else {
                printf("Your guess (Roman) / Ваш ответ (римские): ");
            }

            if (!fgets(buf, sizeof(buf), stdin)) {
                printf("\nEOF, exiting.\n");
                return 0;
            }

            /* Strip trailing newline. */
            nl = strchr(buf, '\n');
            if (nl) {
                *nl = '\0';
            }

            /* Skip empty input. */
            if (buf[0] == '\0') {
                continue;
            }

            if (!g_use_roman) {
                char *endptr = NULL;
                guess = (int)strtol(buf, &endptr, 10);
                if (endptr == buf || *endptr != '\0') {
                    printf("Invalid integer / Неверное целое число.\n");
                    continue;
                }
            } else {
                guess = roman_to_arabic(buf);
                if (guess < 0) {
                    printf("Invalid Roman numeral / Неверное римское число.\n");
                    continue;
                }
            }

            if (guess < MIN_VALUE || guess > MAX_VALUE) {
                if (!g_use_roman) {
                    printf("Out of range [%d, %d] / Вне диапазона [%d, %d].\n",
                           MIN_VALUE, MAX_VALUE, MIN_VALUE, MAX_VALUE);
                } else {
                    printf("Out of range 1..100 / Вне диапазона 1..100.\n");
                }
                continue;
            }

            if (guess < secret) {
                printf("Too small / Мало.\n");
            } else if (guess > secret) {
                printf("Too large / Много.\n");
            } else {
                printf("Correct! / Угадали!\n");
                break;
            }
        }

        return 0;
    }

    /**
     * @brief Program entry point.
     */
    int main(int argc, char **argv)
    {
        int c;
        const char *progname = argv[0];

        static const struct option long_options[] = {
            {"roman",   no_argument,       0, 'r'},
            {"help",    no_argument,       0, 'h'},
            {"help-md", no_argument,       0, 1000},
            {"version", no_argument,       0, 'V'},
            {0, 0, 0, 0}
        };

        while ((c = getopt_long(argc, argv, "rhV", long_options, NULL)) != -1) {
            switch (c) {
            case 'r':
                g_use_roman = 1;
                break;
            case 'h':
                print_help(stdout, progname);
                return 0;
            case 'V':
                printf("%s %s\n", progname, NUMBER_GAME_VERSION);
                return 0;
            case 1000: /* --help-md */
                print_help_md(stdout, progname);
                return 0;
            case '?':
            default:
                print_help(stderr, progname);
                return 1;
            }
        }

        return run_game();
    }
