#define _XOPEN_SOURCE // required for cuserid to work

// includes
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <time.h>

// user defined constants
#define SLEEP_LENGTH 1

int main(void) {
    getP0Info();
    putenv("WHALE=7");
    forkProcesses();
    return 0;
}
// Prints an error message and exits if one occurs. Else, returns the system call value.
int print_if_err(int syscall_val, const char* syscall_name) {
    if (syscall_val < 0) {
        perror(syscall_name);
        exit(errno);
    } else {
        //No syscall error; we can return
        return syscall_val;
    }
}

char* cuserid_wrapper() {
    char* val = cuserid(NULL);
    if (val == NULL) {
        perror("cuserid");
        exit(errno);
    } else {
        return val;
    }
}

time_t time_wrapper() {
    time_t val = time(NULL);
    if (val == ((time_t) -1)) {
        perror("time_wrapper");
        exit(errno);
    } else {
        return val;
    }
}

char* getcwd_wrapper() {
    char* cwd = getcwd(NULL, 0);
    if (cwd == NULL) {
        perror("getcwd_wrapper");
        exit(errno);
    } else {
        return cwd;
    }
}

void getP0Info() {
	printf("P0 Initial Info\n");
    printf("Process ID: %d\n", getpid());
    printf("Parent Process ID: %d\n", getppid());
    
    char hostname[128];
    int host_val = gethostname(hostname, sizeof hostname);
    print_if_err(host_val, "gethostname");
    printf("Hostname: %s\n", hostname);
    printf("Username: %s\n", cuserid_wrapper());

    time_t cur_time = time_wrapper();
    printf("Current time: %s", ctime(&cur_time));
    printf("Current working directory: %s\n", getcwd_wrapper());
}

void forkProcesses() {
    
    pid_t child1_pid = print_if_err(fork(), "fork");
    if (child1_pid == 0) {
        printf("\nChild Process 1\n");
        printf("Process ID: %d\n", getpid());
        printf("Parent Process ID: %d\n", getppid());
        
        sleep(1);
        int whale = atoi(getenv("WHALE"));
        printf("C1: %d\n", whale);
                
    }
    
    pid_t child2_pid = print_if_err(fork(), "fork");
    if (child2_pid == 0) {
        printf("\nChild Process 2\n");
        printf("Process ID: %d\n", getpid());
        printf("Parent Process ID: %d\n", getppid());
    
    }
}
