#include "test.h"

int main(int argc, char **argv) {
    CLIENT *client;

    char *server_hostname = argv[1];

    client = clnt_create(server_hostname, TEST, TEST_VERSION, "udp");

    int number_to_send = atoi(argv[2]);

    int *result = test_function_1(&number_to_send, client);

    printf("received result %d\n", *result);
    clnt_destroy(client);
}
