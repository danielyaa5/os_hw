#include <sys/types.h>
#include "database.h"
#include <unistd.h>

char *generate_id(void) {
    int ret;
    char hostname[60];
    pid_t pid = getpid();
    //hostname[1023] = '\0';
    
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
    struct rpc_args *response = action_1(args, clnt);
    if (response == NULL) {
        clnt_perror(clnt,"expcetion");
        return -1;
    } else {
        printf("Succssful upload\n");
        return 0;// return 0 if successfully get
    }
}

int get(struct rpc_args *args, CLIENT *clnt) {
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
    printf("Action: %s\n", action);
	printf("Message: %s\n", message);
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
    free(args);
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
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
