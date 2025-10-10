CC ?= cc
CFLAGS ?= -O0 -g -Wall -Wextra
LDFLAGS ?=

all: range

range: range.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Run GDB scripts in batch/quiet mode, filter stable lines starting with @@@, compare with expected
test: range
	@echo "[*] Running script1.gdb..."
	@gdb --batch --quiet -x script1.gdb ./range 2>/dev/null | grep '^@@@' > out1.txt || true
	@diff -u expected1.txt out1.txt && echo "script1 OK"
	@echo "[*] Running script2.gdb..."
	@gdb --batch --quiet -x script2.gdb ./range 2>/dev/null | grep '^@@@' > out2.txt || true
	@diff -u expected2.txt out2.txt && echo "script2 OK"

clean:
	rm -f range out1.txt out2.txt

.PHONY: all test clean
