#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define ALPHA_OFFSET 97
#define LETTERS 26

const int NUM_OF_MAPPERS = 4;
const int NUM_OF_REDUCERS = 26;

const int PIPE_READ_END = 0;
const int PIPE_WRITE_END = 1;
const int PIPE_BUFFER_SIZE = 1000;

int mapper_pipes[4][2];
int reducer_pipes[26][2];

void pipe_wrapper(int pipefd[]) {
    int ret = pipe(pipefd);
    if (ret == -1) {
        perror("Error. Failed when trying to create pipes.");
        exit(EXIT_FAILURE);
    }
}

void create_mapper_pipes(void) {
    int i;
    for (i = 0; i < NUM_OF_MAPPERS; i++) {
        pipe_wrapper(mapper_pipes[i]);
    }
}

void create_reducer_pipes(void) {
    int i; 
    for (i=0; i < NUM_OF_REDUCERS; i++) {
        pipe_wrapper(reducer_pipes[i]);
    }
}

// Prints an error msg and exits if one occurs. Else, returns the system call value.
int print_if_err(int syscall_val, const char* syscall_name) {
    if (syscall_val < 0) {
        perror(syscall_name);
        exit(errno);
    } else {
        //No syscall error we can return
        return syscall_val;
    }
}

void fork_mappers(void) {
    

    /* Constants useful to all children */
    char ibuf[PIPE_BUFFER_SIZE]; // input pipe buffer
    int rlen = 0;
    
    int i;
    for (i=0; i<NUM_OF_MAPPERS; i++) {
        pid_t mapper_pid = print_if_err(fork(), "fork");
        if (mapper_pid == 0) {
            rlen = print_if_err(read(mapper_pipes[i][PIPE_READ_END], ibuf, 1000), "read");
            while(rlen > 0) {    
                printf("read line from forked_mappers, p%d: %s\n", i, ibuf);
                rlen = print_if_err(read(mapper_pipes[i][PIPE_READ_END], ibuf, 1000), "read");
            }
            _exit(0);
        }
    }
}

void fork_reducers(void) {
    
    int i;
    for (i = 0; i < NUM_OF_REDUCERS; i++) {
        pid_t reducer_pid = print_if_err(fork(), "fork");
        if (reducer_pid == 0) {
               
        }
    }
}

void send_lines_to_mappers(void) {
    int wlen = 0;
    char obuf[PIPE_BUFFER_SIZE];
    int ob_size;
    int count = 0;

    char buff[BUFFER_SIZE]; // a buffer for each line of the file
    FILE *input_file = fopen("input.txt", "r");
    // read the input file line by line
    while(fgets(buff, BUFFER_SIZE, input_file) > 0) {
        printf("read line from send_lin_to_mappers: %s\n", buff);
        ob_size = sizeof buff;
        switch(count) {
            case 0 :
                write(mapper_pipes[0][PIPE_WRITE_END], buff, ob_size);
                break;
            case 1 : 
                write(mapper_pipes[1][PIPE_WRITE_END], buff, ob_size);
                break;
            case 2 :
                write(mapper_pipes[2][PIPE_WRITE_END], buff, ob_size);
                break;
            case 3 : 
                write(mapper_pipes[3][PIPE_WRITE_END], buff, ob_size);
                break;
            default :
                printf("you did something wrong in send_lines_to_mappers loop");
        }
        count++;
    }
    fclose(input_file);
}

int main(void) {
    // Setup the mapper pipes
    create_mapper_pipes();
    create_reducer_pipes();
    fork_mappers();
    //fork_reducers();
    send_lines_to_mappers();

    return 0;
}
