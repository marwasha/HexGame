CFLAGS = -O3
CC = g++
SRC = main.cpp hexBoard.cpp hexBoard.h
OBJ = $(SRC:.cpp = .o)

debug: CFLAGS = -g
debug: main

main: $(OBJ)
	$(CC) $(CFLAGS) -o main $(OBJ)
clean: rm -f core *.o
