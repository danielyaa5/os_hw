struct rpc_args {
	char action[20];
	char id[80];
    char message[80];
};

program DATABASE {
    version ASSIGNMENT_7 {
    	rpc_args ACTION(struct rpc_args) = 1;
	} = 1;
} = 0x20fff100;
