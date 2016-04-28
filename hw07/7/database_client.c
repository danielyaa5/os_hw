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
    printf("hostname is %sn", hostname);
    printf("The process id is %dn", pid);
    printf("The unique id is %sn", id);
    
    return id;
}

int put(struct rpc_args *args, CLIENT *clnt) {
    struct rpc_args *response = action_1(args, clnt);
    if (response == NULL) {
        clnt_perror(clnt,"expcetion");
        return -1;
        } else {
        printf("Succssful uploadn");
        return 0;// return 0 if successfully get
    }
}

int get(struct rpc_args *args, CLIENT *clnt) {
    struct rpc_args *response = action_1(args, clnt);
    if (response == NULL) {
        clnt_perror(clnt,"expcetion");
        return -1;
        } else {
        printf("Data retrieved: %sn", response->message);// change in the future
        return 0;// return 0 if successfully get
    }
}

void
database_1(char *host, char *action, char *message)
{
    printf("Action: %sn", action);
    printf("Message: %sn", message);
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
    int i;
    for(i = 0; i < 5; i ++){
        char pureMessage[MESSAGE_LEN];
        FILE *file = popen("fortune","r");
        if(fread(pureMessage, MESSAGE_LEN, 1, file) == 0) {
            if(feof(file) == 0) {
                perror("errorfread");
            }
        }
        int r;
        for(r = 0; r < 50; r ++){
            args->messages[i].content[r] = pureMessage[r];
        }
    }
    
    
    int n = 0;
    while(n != 5){
        put(args,clnt,n);
        sleep(1);
        n += 1;
    }
    
    sleep(5); //wait for 5 seconds
    
    int q = 0;
    while(q != 10){
        
        get(args,clnt,q);
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
        printf ("usage: %s server_hostn", argv[0]);
        exit (1);
    }
    host = argv[1];
    action = argv[2];
    message = argv[3];
    
    printf("running client, mainn");
    fflush(stdout);
    database_1 (host, action, message);
    exit (0);
}