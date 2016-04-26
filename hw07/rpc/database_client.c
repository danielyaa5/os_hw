#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_time() {
	time_t current_time;
    char* c_time_string;

    current_time = time(NULL);

    /* Convert to local time format. */
    c_time_string = ctime(&current_time);

    printf("Current time is %s", c_time_string);
}

int main(int argc, char **argv) {
    CLIENT *client;

    char *server_hostname = argv[1];

    client = clnt_create(server_hostname, DATABASE, ASSIGNMENT_7, "udp");

    //rpc arguments struct to pass to server
	struct rpc_args *args = malloc(sizeof(struct rpc_args));

	printf("running client\n");
	//GET message
	if (strcmp(argv[2], "GET") == 0) {

		strcpy(args->action, argv[2]);
		args->id = * argv[3];

		printf("Client %d has issued a GET request\n", args->id);
		struct rpc_args *response = action_1(args, client);

		if (response == NULL) {
			printf("Null response\n");
		} else {
			printf("Null response\n");
		}

	//PUT message
	} else if (strcmp(argv[2], "PUT") == 0) {
		strcpy(args->action, argv[2]);
		args->id = * argv[3];
		strcpy(args->message.content, argv[4]);

		printf("Client %d has issued a PUT request, message = %s\n", args->id, args->message.content);
		struct rpc_args *response = action_1(args, client);

		if (response == NULL) {
			printf("Null response\n");
		} else {
			printf("Null response\n");
		}
	}

	print_time();

    free(args);
    clnt_destroy(client);

    return EXIT_SUCCESS;
}