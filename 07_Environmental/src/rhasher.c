
/*
 * rhasher.c — simple REPL around LibRHash.
 * Rules:
 *   ALG ARG
 *     ALG ∈ {MD5,SHA1,TTH,...}, case determines output encoding:
 *       - Uppercase first letter -> HEX (uppercase)
 *       - Lowercase first letter -> Base64
 *     ARG: "string" (no spaces; leading quote indicates string) OR filename.
 *
 * Build-time:
 *   - If USE_READLINE is defined (by configure or CFLAGS), use GNU readline;
 *     otherwise use POSIX getline().
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

#include <rhash.h>

#ifdef USE_READLINE
  #include <readline/readline.h>
  #include <readline/history.h>
#endif

static unsigned map_alg(const char* name) {
    if (!name || !*name) return 0;
    // Accept common spellings
    if (strcasecmp(name, "MD5") == 0 || strcasecmp(name, "md5") == 0) return RHASH_MD5;
    if (strcasecmp(name, "SHA1") == 0 || strcasecmp(name, "sha1") == 0) return RHASH_SHA1;
    if (strcasecmp(name, "TTH") == 0 || strcasecmp(name, "tth") == 0) return RHASH_TTH;
    // Optionally allow more (not required by assignment)
    if (strcasecmp(name, "SHA256") == 0) return RHASH_SHA256;
    if (strcasecmp(name, "SHA512") == 0) return RHASH_SHA512;
    return 0;
}

static void do_hash(unsigned alg_id, const char* alg_token, const char* arg) {
    if (!alg_id) {
        fprintf(stderr, "unknown algorithm: %s\n", alg_token ? alg_token : "(null)");
        return;
    }
    if (!arg || !*arg) {
        fprintf(stderr, "usage: ALG FILE|\"string\"\n");
        return;
    }
    // Decide output format by case of first letter
    int flags = isupper((unsigned char)alg_token[0]) ? (RHPR_HEX | RHPR_UPPERCASE) : RHPR_BASE64;

    unsigned char digest[128];
    char out[256];

    int rc;
    if (arg[0] == '\"') {
        // treat as string literal without spaces (assignment assumption)
        size_t len = strlen(arg);
        // strip leading quote and optional trailing quote
        const char* s = arg + 1;
        size_t slen = len >= 2 && arg[len-1] == '\"' ? (len - 2) : (len - 1);
        rc = rhash_msg(alg_id, s, slen, digest);
        if (rc < 0) {
            fprintf(stderr, "rhash_msg failed\n");
            return;
        }
    } else {
        rc = rhash_file(alg_id, arg, digest);
        if (rc < 0) {
            fprintf(stderr, "file error: %s: %s\n", arg, strerror(errno));
            return;
        }
    }
    int dsize = rhash_get_digest_size(alg_id);
    rhash_print_bytes(out, digest, dsize, flags);
    // print digest only
    fputs(out, stdout);
    fputc('\n', stdout);
    fflush(stdout);
}

static char* next_line(bool interactive) {
#ifdef USE_READLINE
    const char* prompt = interactive ? "rhash> " : "";
    char* line = readline(prompt);
    if (line && *line) add_history(line);
    return line; // malloc'd by readline
#else
    if (interactive) {
        // prompt to stderr to keep stdout clean for tests/pipes
        fputs("rhash> ", stderr);
        fflush(stderr);
    }
    char* line = NULL;
    size_t cap = 0;
    ssize_t n = getline(&line, &cap, stdin);
    if (n < 0) {
        free(line);
        return NULL;
    }
    // strip trailing newline
    if (n && line[n-1] == '\n') line[n-1] = '\0';
    return line; // malloc'd by getline
#endif
}

int main(void) {
    rhash_library_init();

    bool interactive = isatty(STDIN_FILENO);
    for (;;) {
        char* line = next_line(interactive);
        if (!line) break; // EOF (Ctrl+D)

        // For getline branch, trailing newline is stripped above.
#ifdef USE_READLINE
        // readline doesn't include newline; ensure not NULL
#endif
        // tokenize
        char* save = NULL;
        char* alg = strtok_r(line, " \t\r\n", &save);
        char* arg = strtok_r(NULL, " \t\r\n", &save);

        if (alg && alg[0] != '\0') {
            unsigned alg_id = map_alg(alg);
            do_hash(alg_id, alg, arg);
        } // else ignore empty lines

#ifdef USE_READLINE
        free(line);
#else
        free(line);
#endif
    }
    return 0;
}
