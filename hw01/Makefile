C = gcc
C_FLAGS = -Wall -Wextra

all: eecs338_hw01

eecs338_hw01: eecs338_hw01.o
		$(CC) eecs338_hw01.o -o eecs338_hw01

eecs338_hw01.o: eecs338_hw01.c
		$(CC) -c $(C_FLAGS) eecs338_hw01.c

clean:
		rm -f eecs338_hw01 eecs338_hw01.o
