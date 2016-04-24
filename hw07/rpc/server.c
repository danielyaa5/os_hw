#include "database.h"

int *test_function_1_svc(int *input, struct svc_req *request) {
    int response = *input + 1;

    printf("received %d, responding with %d\n", *input, response);

    return(&response);
}
