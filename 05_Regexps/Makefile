SHELL := /bin/bash
# Makefile — 05_Regexps
# Targets: all (esub), clean, test
CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -std=c11 -pedantic

BIN := esub

all: $(BIN)

$(BIN): esub.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(BIN) out1.txt out2.txt
	find tests -type f -name '*.sh' -exec chmod +x {} \;

test: $(BIN) tests/sed_once.sh tests/cases.tsv tests/error_cases.tsv
	@bash tests/run_tests.sh
