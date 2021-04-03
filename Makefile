TARGET_EXEC := red-black-tree-example
CC := gcc

$(TARGET_EXEC): main.c
	$(CC) -Wall -std=c2x -o $@ *.c -lm
