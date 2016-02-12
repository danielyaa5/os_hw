#include <sys/wait.h>
#include <stdbool.h>
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

void send_chars_to_reducers(char * line) {
    printf("SEND_CHARS_TO_REDUCERS read: %s\n\n", line);
    int i;
    int ob_size = 1;
    int wlen = 0;
    for (i = 0; i < strlen(line); i++) {
        if (line[i] >= ALPHA_OFFSET && line[i] < ALPHA_OFFSET + LETTERS) {
            int pipe_num = line[i] - ALPHA_OFFSET;
            printf("SENDING %c TO REDUCER PIPE %d\n", line[i], pipe_num);
            wlen = print_if_err(write(reducer_pipes[pipe_num][PIPE_WRITE_END], &line[i], ob_size), "write");
            printf("WROTE %s to REDUCER %d\n", line[i], i);
        }
    }
}

void close_reducer_pipes(void) {
    int i;
    for (i = 0; i < NUM_OF_REDUCERS; i++) {
        close(reducer_pipes[i][PIPE_WRITE_END]);
        close(reducer_pipes[i][PIPE_READ_END]);
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
            int j;
            for (j=0; j < NUM_OF_MAPPERS; j++) {
                close(mapper_pipes[i][PIPE_WRITE_END]);
                if (j != i) {
                    close(mapper_pipes[j][PIPE_READ_END]);
                }
            }
            rlen = print_if_err(read(mapper_pipes[i][PIPE_READ_END], ibuf, 1000), "read");
            send_chars_to_reducers(ibuf);
            close_reducer_pipes(); 
            //printf("forked mapper%d read: %s\n\n", i, ibuf);
            close(mapper_pipes[i][PIPE_READ_END]);
            _exit(0);
        }
    }
}

void fork_reducers(void) {
    printf("HELLLOOOO FROM REDUCER\n"); 
    char ibuf[PIPE_BUFFER_SIZE]; // input pipe buffer
    int rlen = 0;
    int i;
    for (i = 0; i < NUM_OF_REDUCERS; i++) {
        pid_t reducer_pid = print_if_err(fork(), "fork");
        if (reducer_pid == 0) {
            while (1) {
                rlen = print_if_err(read(reducer_pipes[i][PIPE_READ_END], ibuf, 1), "read");
                if (rlen > 0) {
                   printf("REDUCER #%d, read %s\n", i, ibuf);
                } 
            }       
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
        //printf("send_lines_to_mappers read: %s\n\n", buff);
        ob_size = sizeof buff;
        switch(count) {
            case 0 :
                write(mapper_pipes[0][PIPE_WRITE_END], buff, ob_size);
                close(mapper_pipes[0][PIPE_WRITE_END]);
                close(mapper_pipes[0][PIPE_READ_END]);
                break;
            case 1 : 
                write(mapper_pipes[1][PIPE_WRITE_END], buff, ob_size);
                close(mapper_pipes[1][PIPE_WRITE_END]);
                close(mapper_pipes[1][PIPE_READ_END]);
                break;
            case 2 :
                write(mapper_pipes[2][PIPE_WRITE_END], buff, ob_size);
                close(mapper_pipes[2][PIPE_WRITE_END]);
                close(mapper_pipes[2][PIPE_READ_END]);
                break;
            case 3 : 
                write(mapper_pipes[3][PIPE_WRITE_END], buff, ob_size);
                close(mapper_pipes[3][PIPE_WRITE_END]);
                close(mapper_pipes[3][PIPE_READ_END]);
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
    fork_reducers();
    fork_mappers();
    send_lines_to_mappers();

    return 0;
}
