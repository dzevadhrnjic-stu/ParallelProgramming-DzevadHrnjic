CC = gcc
CFLAGS = -O2 -Wall -Wextra -Wpedantic -std=c11
TARGET = sum_benchmark
SOURCES = main.c sum_algorithms.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@

%.o: %.c sum_algorithms.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) 1048576 | tee results.txt

clean:
	rm -f $(TARGET) $(OBJECTS) results.txt
