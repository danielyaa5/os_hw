#include "main.h"

void hydrogen(void);//executes hydrogen.c
void carbon(void);//executes carbon.c

struct threadInfo {
	int threadId;
};

struct threadInfo hydrogenIDs[NUM_H];
struct threadInfo carbonIDs[NUM_C];

int main() {
	int semid, shmid;//semaphore memory id, shared memory id
	unsigned short seminit[NUM_SEMS];//used to initialize semaphores
	struct common *shared;//pointer to shared data structure
	union semun semctlarg;//used to initialize semaphores

	pthread_t hydrogen[NUM_H];
	pthread_t carbon[NUM_C];
	pthread_attr_t attr;
	void *exit_status;

	//Creating a set of attributes to send to the threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	//get semaphore memory id
	if ((semid = semget(SEMKEY, NUM_SEMS, IPC_CREAT|0777)) < 0) {
		perror("semget");
		exit(EXIT_FAILURE);
	}

	printf("THE PROGRAM IS STARTING\n\n");

	seminit[MUTEX] = 1;//initialize mutex semaphore count to 1
	seminit[SH] = 0;//initialize hyrdrogen semaphore count to 0
	seminit[SC] = 0;//initialize carbon semaphore count to 0
	semctlarg.array = seminit;//set control array

	//apply initialization
	if ((semctl(semid, NUM_SEMS, SETALL, semctlarg)) < 0) {
		perror("semctl");
		exit(EXIT_FAILURE);
	}

	//get shared memory id
	if ((shmid = shmget(SHMKEY, 1*K, IPC_CREAT|0777)) < 0) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	//retrieve pointer to shared data structure
	if ((shared = (struct common *)shmat(shmid, 0, 0)) < 0) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

	//initialize shared data structure variables
	shared->waiting_H = 0;
	shared->waiting_C = 0;

	int retVal;//used to check return value of fork()

	// spawn 20 Hydrogens
	for (int i=0; i<NUM_H; i++) {
		// if ((retVal = fork()) == 0) {
		// 	hydrogen();
		// 	fflush(stdout);
		// 	printf("New Hydrogen process created\n");
		// 	fflush(stdout);
		// } else if (retVal < 0) {
		// 	perror("fork");
		// 	exit(EXIT_FAILURE);
		// }
		hydrogenIDs[i].threadId = i;
		retVal = pthread_create(&hydrogen[i], &attr, hydrogen, (void*) &hydrogenIDs[i]);
		if (retVal == 0) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	// spawn 5 Carbons
	for (int i=0; i<NUM_C; i++) {
		// if ((retVal = fork()) == 0) {
		// 	carbon();
		// 	fflush(stdout);
		// 	printf("New Hydrogen process created\n");
		// 	fflush(stdout);
		// } else if (retVal < 0) {
		// 	perror("fork");
		// 	exit(EXIT_FAILURE);
		// }
		carbonIDs[i].threadId = i;
		retVal = pthread_create(&carbon[i], &attr, carbon, (void*) &carbonIDs[i]);
		if (retVal == 0) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	//wait for all car processes to finish
	for (int i = 0; i < 25; ++i) {
		if (wait(0) < 0) {
			perror("wait");
			exit(EXIT_FAILURE);
		}
	}

	printf("All atoms have crossed!\n");

		//delete semaphores
	if (semctl(semid, NUM_SEMS, IPC_RMID, 0) < 0) {
		perror("semctl");
		exit(EXIT_FAILURE);
	}

	//delete shared memory
	if (shmctl(shmid, IPC_RMID, 0) < 0) {
		perror("shmctl");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}

void carbon(void) {
	execl("carbon", "carbon", 0);
	perror("execl");
	exit(EXIT_FAILURE);//if exec returns there was an error
}

void hydrogen(void) {
	execl("hydrogen", "hydrogen", 0);
	perror("execl");
	exit(EXIT_FAILURE);//if exec returns there was an error
}