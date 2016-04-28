#include <sys/types.h>
#include "database.h"
#include <unistd.h>

char *generate_id(void) {
    int ret;
    char hostname[60];
    pid_t pid = getpid();
    //hostname[1023] = '';
    
    if ((ret = gethostname(hostname,60) < 0)) {
        perror("gethostname");
        exit(EXIT_FAILURE);
    }
    
    int size = snprintf(NULL, 0, "%s%d", hostname, pid);
    char * id = malloc(size + 1);
    sprintf(id, "%s%d", hostname, pid);
    printf("hostname is %s\n", hostname);
    printf("The process id is %d\n", pid);
    printf("The unique id is %s\n", id);
    
    return id;
}

int put(struct rpc_args *args, CLIENT *clnt) {
    printf("Client PUT: %s\n", args->message);
    struct rpc_args *response = action_1(args, clnt);
    if (response == NULL) {
        clnt_perror(clnt,"expcetion");
        printf("PUT fail\n");
        return -1;
    } else {
        printf("Succssful upload\n");
        return 0;// return 0 if successfully get
    }
}

int get(struct rpc_args *args, CLIENT *clnt) {
    printf("Client GET\n");
    struct rpc_args *response = action_1(args, clnt);
    if (response == NULL) {
        clnt_perror(clnt,"expcetion");
        return -1;
    } else {
        printf("Data retrieved: %s\n", response->message);// change in the future
        return 0;// return 0 if successfully get
    }
}

void
database_1(char *host, char *action, char *message)
{
    CLIENT *clnt;
    rpc_args  *result_1;
    //struct rpc_args  action_1_arg;
    
    clnt = clnt_create (host, DATABASE, ASSIGNMENT_7, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror (host);
        exit (1);
    }
    //rpc arguments struct to pass to server
    struct rpc_args *args = malloc(sizeof(struct rpc_args));
    
    char *id = generate_id();
    strcpy(args->id, id);
    strcpy(args->action, "PUT"); 
    int n = 0;
    while(n != 5){
        char pureMessage[80];
        pureMessage[79] = '\0';
        FILE *file = popen("fortune","r");
        if (file == NULL) {
            perror("errorpopen");
        }
        if(fread(pureMessage, 79, 1, file) == 0) {
            if(feof(file) == 0) {
                perror("errorfread");
            }
        }
        strcpy(args->message, pureMessage);
        put(args,clnt);
        sleep(1);
        n += 1;
    }
    
    sleep(5); //wait for 5 seconds
    
    strcpy(args->action, "GET");
    int q = 0;
    while(q != 10){
        get(args,clnt);
        sleep(1);
        q += 1;
    }
    free(args);
    #ifndef    DEBUG
    clnt_destroy (clnt);
    #endif     /* DEBUG */
}


int
main (int argc, char *argv[])
{
    char *host;
    char *action;
    char *message;
    
    if (argc < 2) {
        printf ("usage: %s server_host\n", argv[0]);
        exit (1);
    }
    host = argv[1];
    action = argv[2];
    message = argv[3];
    
    printf("running client, main\n");
    fflush(stdout);
    database_1 (host, action, message);
    exit (0);
}
