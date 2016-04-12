
all: carbon.c hydrogen.c main.c main.h
	gcc -Wno-cpp -std=c99 -o carbon carbon.c
	gcc -Wno-cpp -std=c99 -o hydrogen hydrogen.c
	gcc -Wno-cpp -std=c99 -g -o main main.c
	chmod 755 main

clean:
	-rm carbon hydrogen main
	-rm -r main.dSYM
