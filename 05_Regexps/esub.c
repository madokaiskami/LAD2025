// esub.c — LinuxApplicationDevelopment2025 / 05_Regexps
// Implements: esub REGEXP SUBSTITUTION STRING
// Behavior: like `echo "STRING" | sed -E 's/REGEXP/SUBSTITUTION/'` (single replacement).
// Requirements satisfied:
//  - Extended regex (REG_EXTENDED)
//  - Diagnostics for invalid regex via regerror
//  - SUBSTITUTION parses backreferences \1..\9 (max 9 groups), up to 100 occurrences
//  - Non-existent group reference is an error
//  - "\\" in SUBSTITUTION becomes "\"
//  - Prints original string when no match (like sed single substitution)
//  - Robust error checking
//
// Build: cc -O2 -Wall -Wextra -std=c11 -pedantic -o esub esub.c
// Usage: ./esub '([0-9]+)' 'X\\1Y' 'abc123def'   => abcX123Ydef

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

#define MAX_GROUPS 10          // 0..9 (0 = whole match, 1..9 = up to 9 capture groups)
#define MAX_REF_OCCURRENCES 100

static void die_usage(const char *prog) {
    fprintf(stderr,
            "Usage: %s REGEXP SUBSTITUTION STRING\n"
            "       (single replacement, extended regex)\n",
            prog);
    exit(2);
}

static void regerror_die(int code, const regex_t *re, const char *where) {
    char buf[256];
    regerror(code, re, buf, sizeof(buf));
    if (where && *where)
        fprintf(stderr, "esub: %s: %s\n", where, buf);
    else
        fprintf(stderr, "esub: %s\n", buf);
    exit(2);
}

struct sbuf {
    char *data;
    size_t len;
    size_t cap;
};

static void sbuf_init(struct sbuf *b) {
    b->data = NULL;
    b->len = 0;
    b->cap = 0;
}

static void sbuf_reserve(struct sbuf *b, size_t add) {
    if (b->len + add + 1 <= b->cap) return;
    size_t ncap = b->cap ? b->cap : 64;
    while (ncap < b->len + add + 1) ncap *= 2;
    char *nd = realloc(b->data, ncap);
    if (!nd) {
        fprintf(stderr, "esub: out of memory\n");
        exit(2);
    }
    b->data = nd;
    b->cap = ncap;
}

static void sbuf_append_mem(struct sbuf *b, const char *s, size_t n) {
    if (n == 0) return;
    sbuf_reserve(b, n);
    memcpy(b->data + b->len, s, n);
    b->len += n;
    b->data[b->len] = '\0';
}

static void sbuf_append_char(struct sbuf *b, char c) {
    sbuf_reserve(b, 1);
    b->data[b->len++] = c;
    b->data[b->len] = '\0';
}

static void expand_substitution(
    const char *sub, const char *src,
    const regmatch_t pmatch[MAX_GROUPS],
    size_t groups_in_regex, // re->re_nsub
    struct sbuf *out
) {
    size_t ref_count = 0;
    for (size_t i = 0; sub[i] != '\0'; ++i) {
        if (sub[i] != '\\') {
            sbuf_append_char(out, sub[i]);
            continue;
        }
        // sub[i] is backslash, look ahead
        char next = sub[i+1];
        if (next == '\0') {
            // Trailing backslash -> literal backslash
            sbuf_append_char(out, '\\');
            break;
        }
        if (next == '\\') {
            // "\\" -> "\"
            sbuf_append_char(out, '\\');
            i++; // consume next
            continue;
        }
        if (next >= '1' && next <= '9') {
            int idx = next - '0'; // single digit only; \11 == \1 + '1'
            if ((size_t)idx > groups_in_regex) {
                fprintf(stderr, "esub: substitution refers to non-existent group \\%d (pattern has %zu group(s))\n",
                        idx, groups_in_regex);
                exit(2);
            }
            if (++ref_count > MAX_REF_OCCURRENCES) {
                fprintf(stderr, "esub: too many backreference occurrences in substitution (>%d)\n",
                        MAX_REF_OCCURRENCES);
                exit(2);
            }
            // If the (existing) group didn't participate in the match, it's an empty string.
            if (pmatch[idx].rm_so >= 0 && pmatch[idx].rm_eo >= pmatch[idx].rm_so) {
                sbuf_append_mem(out, src + pmatch[idx].rm_so,
                                     (size_t)(pmatch[idx].rm_eo - pmatch[idx].rm_so));
            }
            i++; // consume digit
            continue;
        }
        // Unknown escape -> treat as escaping of the next char (drop the backslash).
        sbuf_append_char(out, next);
        i++; // consume next
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        die_usage(argv[0]);
    }
    const char *pattern = argv[1];
    const char *subst   = argv[2];
    const char *input   = argv[3];

    regex_t re;
    int ccode = regcomp(&re, pattern, REG_EXTENDED);
    if (ccode != 0) {
        regerror_die(ccode, &re, "regcomp");
    }

    regmatch_t pmatch[MAX_GROUPS];
    int ecode = regexec(&re, input, MAX_GROUPS, pmatch, 0);
    if (ecode == REG_NOMATCH) {
        // No match -> print the original string unchanged (sed behavior)
        puts(input);
        regfree(&re);
        return 0;
    } else if (ecode != 0) {
        regerror_die(ecode, &re, "regexec");
    }

    // Build: prefix + expanded_subst + suffix
    struct sbuf out; sbuf_init(&out);
    // prefix
    if (pmatch[0].rm_so > 0) {
        sbuf_append_mem(&out, input, (size_t)pmatch[0].rm_so);
    }
    // expanded substitution
    expand_substitution(subst, input, pmatch, re.re_nsub, &out);
    // suffix
    if (pmatch[0].rm_eo >= 0) {
        size_t tail = strlen(input) - (size_t)pmatch[0].rm_eo;
        sbuf_append_mem(&out, input + pmatch[0].rm_eo, tail);
    }
    puts(out.data ? out.data : "");

    free(out.data);
    regfree(&re);
    return 0;
}
