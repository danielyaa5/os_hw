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

int main(void) {
    getP0Info();
    putenv("WHALE=7");
    forkProcesses();
    
    int whale = atoi(getenv("WHALE"));
    sleep(1);
    printf("PO: %d shrimp (WHALE environment variable is now %d)\n", whale, whale);
    char new_whale[50];

    while (whale > 1) {
        sleep(3);
        whale = whale - 3;
        sprintf(new_whale, "%d", whale);
        setenv("WHALE", new_whale, 1);
        printf("PO: %d shrimp (WHALE environment variable is now %d)\n", whale, whale);
    }

    pid_t pid;
    while (pid = waitpid(-1, NULL, 0)) {
        if (errno == ECHILD) {
            perror("waitpid");
            break;
        }
    }
    whale = 0;
    sprintf(new_whale, "%d", whale);
    setenv("WHALE", new_whale, 1);
    printf("PO: %d shrimp (WHALE environment variable is now %d)\n", whale, whale);
    fflush(stdout);
    fflush(stderr);
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
        fflush(stdout);
        fflush(stderr);
        sleep(2);
        int whale = 6;
        char new_whale[50];
        sprintf(new_whale, "%d", whale);
        setenv("WHALE", new_whale, 1);
        printf("C1: %d shrimp (WHALE environment variable is now %d)\n", whale, whale);
        fflush(stderr);
        fflush(stdout);
        while(whale > 3) {
            sleep(3);
            whale = whale - 3;
            sprintf(new_whale, "%d", whale);
            setenv("WHALE", new_whale, 1);
            printf("C1: %d shrimp (WHALE environment variable is now %d)\n", whale, whale);
        }
        
        fflush(stderr);
        fflush(stdout);
        sleep(3);
        printf("\n");
        print_if_err(chdir("/"), "chdir");
        printf("C1: executing ls command\n");
        execl("/bin/ls", "ls", "-l", "-a", NULL);
        fflush(stdout);
        fflush(stderr);
        _exit(0);                
    }
        
    pid_t child2_pid = print_if_err(fork(), "fork");
    if (child2_pid == 0) {
        printf("\nChild Process 2\n");
        printf("Process ID: %d\n", getpid());
        printf("Parent Process ID: %d\n", getppid());
    
        sleep(3);
        int whale = 5;
        char new_whale[50];
        sprintf(new_whale, "%d", whale);
        setenv("WHALE", new_whale, 1);
        printf("C2: %d shrimp (WHALE environment variable is now %d)\n", whale, whale);        
        
        while(whale > 2) {
            sleep(3);
            whale = whale - 3;
            sprintf(new_whale, "%d", whale);
            setenv("WHALE", new_whale, 1);
            printf("C2: %d shrimp (WHALE environment variable is now %d)\n", whale, whale);    
        }
        
        sleep(2);
        printf("C2: getting cwd\n");
        printf("C2 CWD: %s\n", getcwd_wrapper());
        fflush(stdout);
        fflush(stderr);
        _exit(0);
    }
}
