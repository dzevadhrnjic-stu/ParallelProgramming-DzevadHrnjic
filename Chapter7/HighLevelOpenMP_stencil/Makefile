CC = gcc
CFLAGS = -O2 -fopenmp -Wall -Wextra -std=c11 -march=native
LDFLAGS = -lm

ifeq ($(OS),Windows_NT)
EXE = .exe
RUN =
DELETE = del /Q
DELETE_REDIRECT = 2>NUL
else
EXE =
RUN = ./
DELETE = rm -f
DELETE_REDIRECT =
endif

PROGRAMS = stencil_opt2$(EXE) stencil_opt4$(EXE) stencil_opt6$(EXE)
UTIL_OBJECTS = timer.o malloc2D.o

.PHONY: all clean rebuild run run-opt2 run-opt4 run-opt6

all: $(PROGRAMS)
	@echo "All stencil programs built successfully."

%$(EXE): %.c $(UTIL_OBJECTS)
	$(CC) $(CFLAGS) $< $(UTIL_OBJECTS) -o $@ $(LDFLAGS)

timer.o: timer.c timer.h
	$(CC) $(CFLAGS) -c timer.c -o timer.o

malloc2D.o: malloc2D.c malloc2D.h
	$(CC) $(CFLAGS) -c malloc2D.c -o malloc2D.o

run: all
	@echo "Running stencil_opt2"
	$(RUN)stencil_opt2$(EXE)
	@echo
	@echo "Running stencil_opt4"
	$(RUN)stencil_opt4$(EXE)
	@echo
	@echo "Running stencil_opt6"
	$(RUN)stencil_opt6$(EXE)

run-opt2: stencil_opt2$(EXE)
	$(RUN)stencil_opt2$(EXE)

run-opt4: stencil_opt4$(EXE)
	$(RUN)stencil_opt4$(EXE)

run-opt6: stencil_opt6$(EXE)
	$(RUN)stencil_opt6$(EXE)

clean:
	-$(DELETE) $(PROGRAMS) $(UTIL_OBJECTS) $(DELETE_REDIRECT)

rebuild: clean all
