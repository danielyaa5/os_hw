struct message {
	char content[2000];
};

struct rpc_args {
	char action[20];
	int id;
	struct message message;
};

program DATABASE {
    version ASSIGNMENT_7 {
    	rpc_args ACTION(struct rpc_args) = 1;
	} = 1;
} = 0x20fff100;

