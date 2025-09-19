CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lncurses
TARGET = Show
SRC = Show.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET) *~

.PHONY: all clean
