CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

.PHONY: all clean doc man check

all: number-game

number-game: number-game.c roman.c roman.h
	$(CC) $(CFLAGS) -o $@ number-game.c roman.c

doc: Doxyfile number-game.c roman.c roman.h doc_mainpage.dox
	doxygen Doxyfile

man: number-game
	help2man -N -o number-game.1 ./number-game

check: all
	./number-game --help > /dev/null
	./number-game --help-md > /dev/null

clean:
	rm -f number-game number-game.1
	rm -rf doxygen-doc
