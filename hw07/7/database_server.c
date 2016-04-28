/*
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "database.h"

// used to store messages.
struct server_message {
    char message[80];
    char id[80];
    struct server_message *next_msg;
};

//head of the list
static struct server_message *first_message = NULL;

//insert the message into the specified user's messages
void insert_message(char *id, char *message);
//retrieve the message from the specified user
char *get_message(char *id);

rpc_args *
action_1_svc(struct rpc_args *args, struct svc_req *rqstp)
{
    printf("Running server function, action: %s\n", args->action);

	rpc_args *result = malloc(sizeof(struct rpc_args));

	/*
	 * insert server code here
	 */
 
    if (strcmp(args->action, "GET") == 0) {
        printf("User %s, is requesting a message from the server\n", args->id);
        char *got_msg = get_message(args->id);
        printf("Retrieved message for %s: %s", args->id, got_msg);
        strcpy(result->message, got_msg);
    } else if (strcmp(args->action, "PUT") == 0) {
        printf("User %s, is putting a message, %s\n", args->id, args->message);
        insert_message(args->id, args->message);
    }


	return result;
}

void insert_message(char *id, char *message) {
    if (first_message == NULL) {
        first_message = malloc( sizeof(struct server_message) );
        strcpy(first_message->id, id);
        strcpy(first_message->message, message);
        first_message->next_msg = NULL;
    } else {
        struct server_message *curr_msg = first_message;

        while (curr_msg->next_msg != NULL) {
            curr_msg = curr_msg->next_msg;
        }

        curr_msg->next_msg = malloc( sizeof(struct server_message) );
        strcpy (curr_msg->next_msg->id, id);
        strcpy (curr_msg->next_msg->message, message);
        curr_msg->next_msg->next_msg = NULL;
    }
}

char *get_message(char *id) {
    char *message;
    char *not_found =(char *)  malloc( sizeof(char) * 20 );
    not_found = "No messages available";
    struct server_message *curr_msg = first_message;
    struct server_message *prev_msg = first_message;

    if (curr_msg != NULL && (strcmp(curr_msg->id, id) != 0)) {
        strcpy (message, curr_msg->message);
        //Remove message
        first_message = NULL;
        return message;
    }

    curr_msg = curr_msg->next_msg;

    while (curr_msg != NULL) {
        if (strcmp(curr_msg->id, id) != 0) {
            strcpy (message, curr_msg->message);
            //Remove message
            prev_msg->next_msg = curr_msg->next_msg;
            return message;
        } else {
            prev_msg = curr_msg;
            curr_msg = curr_msg->next_msg;
        }
    }
    return not_found;
}
