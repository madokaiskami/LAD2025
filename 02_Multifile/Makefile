.DEFAULT_GOAL := all

CC      ?= cc
AR      ?= ar
RANLIB  ?= ranlib

CFLAGS  ?= -O2 -g -Wall -Wextra -std=c11
PICFLAGS = -fPIC
LDFLAGS ?= 
SHLDFLAGS = -shared

SRC_DIR := src
BUILD   := build
OUTDIR  := out

SRCS := $(SRC_DIR)/prog.c $(SRC_DIR)/fun.c $(SRC_DIR)/const.c
HDRS := $(SRC_DIR)/output.h

OBJS := $(BUILD)/prog.o $(BUILD)/fun.o $(BUILD)/const.o
POBJS := $(BUILD)/fun.pic.o $(BUILD)/const.pic.o

LIBSTATIC := $(BUILD)/liboutput_static.a
LIBSHARED := $(BUILD)/liboutput.so

BIN_PROG  := $(BUILD)/prog
BIN_A     := $(BUILD)/prog-a
BIN_SO    := $(BUILD)/prog-so

$(BUILD) $(OUTDIR):
	mkdir -p $@

$(BUILD)/%.o: $(SRC_DIR)/%.c $(HDRS) | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.pic.o: $(SRC_DIR)/%.c $(HDRS) | $(BUILD)
	$(CC) $(CFLAGS) $(PICFLAGS) -c $< -o $@

$(LIBSTATIC): $(BUILD)/fun.o $(BUILD)/const.o | $(BUILD)
	$(AR) rcs $@ $^
	$(RANLIB) $@

$(LIBSHARED): $(POBJS) | $(BUILD)
	$(CC) $(SHLDFLAGS) -Wl,-soname,liboutput.so -o $@ $^

$(BIN_PROG): $(BUILD)/prog.o $(BUILD)/fun.o $(BUILD)/const.o | $(BUILD)
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN_A): $(BUILD)/prog.o $(LIBSTATIC) | $(BUILD)
	$(CC) $(LDFLAGS) -o $@ $(BUILD)/prog.o -L$(BUILD) -loutput_static

$(BIN_SO): $(BUILD)/prog.o $(LIBSHARED) | $(BUILD)
	$(CC) $(LDFLAGS) -Wl,-rpath,'$$ORIGIN' -o $@ $(BUILD)/prog.o -L$(BUILD) -loutput

.PHONY: all
all: $(LIBSTATIC) $(LIBSHARED) $(BIN_PROG) $(BIN_A) $(BIN_SO)

.PHONY: test
test: all | $(OUTDIR)
	@set -e; \
	echo "[TEST] Running binaries and capturing output in '$(OUTDIR)'..."; \
	for B in "$(BIN_PROG)" "$(BIN_A)" "$(BIN_SO)"; do \
	  BN=$$(basename $$B); \
	  echo "  -> $$BN (no args)";  "$$B"            > "$(OUTDIR)/$${BN}_0.txt" 2>&1 || true; \
	  echo "  -> $$BN (1 arg)";   "$$B" one        > "$(OUTDIR)/$${BN}_1.txt" 2>&1 || true; \
	  echo "  -> $$BN (3 args)";  "$$B" a b c      > "$(OUTDIR)/$${BN}_3.txt" 2>&1 || true; \
	done; \
	echo "[TEST] Comparing outputs (prog vs prog-a / prog-so)"; \
	for SUF in 0 1 3; do \
	  cmp -s "$(OUTDIR)/prog_$$SUF.txt"   "$(OUTDIR)/prog-a_$$SUF.txt" || { echo "Mismatch: prog vs prog-a (case $$SUF)"; exit 1; }; \
	  cmp -s "$(OUTDIR)/prog_$$SUF.txt"   "$(OUTDIR)/prog-so_$$SUF.txt" || { echo "Mismatch: prog vs prog-so (case $$SUF)"; exit 1; }; \
	done; \
	echo "[TEST] OK: outputs match for all cases."

.PHONY: clean
clean:
	$(RM) -r $(BUILD) $(OUTDIR)

.PHONY: run
run: $(BIN_PROG)
	@$(BIN_PROG)

.PHONY: rebuild
rebuild: clean all
