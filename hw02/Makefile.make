C = gcc
C_FLAGS = -Wall -Wextra

all: eecs338_hw02

eecs338_hw02: eecs338_hw02.o
		$(CC) eecs338_hw02.o -o eecs338_hw02

eecs338_hw02.o: eecs338_hw02.c
		$(CC) -c $(C_FLAGS) eecs338_hw02.c

clean:
		rm -f eecs338_hw02 eecs338_hw02.o
