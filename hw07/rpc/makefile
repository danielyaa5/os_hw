all:
	gcc -o server server.c database_svc.c
	gcc -o client client.c database_clnt.c

clean:
	-rm server client database_svc.c database_clnt.c database.h
	-rm *~
