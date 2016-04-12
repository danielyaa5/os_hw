#include "main.h"

void *start_hydrogen(void *);//executes hydrogen.c
void *start_carbon(void *);//executes carbon.c

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
		retVal = pthread_create(&hydrogen[i], &attr, start_hydrogen, (void*) &hydrogenIDs[i]);
		if (retVal != 0) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	printf("this line reached\n");

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
		retVal = pthread_create(&carbon[i], &attr, start_carbon, (void*) &carbonIDs[i]);
		if (retVal != 0) {
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	//wait for all car processes to finish
	// for (int i = 0; i < 25; ++i) {
	// 	if (wait(0) < 0) {
	// 		perror("wait");
	// 		exit(EXIT_FAILURE);
	// 	}
	// }

	//Wait for all the threads to finish
	for(int i = 0; i < NUM_C; i++)
	{
		pthread_join(carbon[i], &exit_status);
	}

	for(int i = 0; i < NUM_H; i++)
	{
		pthread_join(hydrogen[i], &exit_status);
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

void *start_hydrogen(void* arg) {
	printf("start_hydrogen called\n");
	struct common *shared;//pointer to shared data structure

	int semid, shmid;//semaphore memory id, shared memory id

	int pid = getpid();

	//get semaphore memory id
	if ((semid = semget(SEMKEY, NUM_SEMS, 0777)) < 0) {
		perror("semget");
		exit(EXIT_FAILURE);
	}

	//get shared memory id
	if ((shmid = shmget(SHMKEY, 1*K, 0777)) < 0) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	//get pointer to shared data structure
	if ((shared = (struct common *)shmat(shmid, 0, 0)) < 0) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

	// acquire lock on mutex before accessing shared memory
	semWait(semid, MUTEX);

	fflush(stdout);
	printf("Hydrogen atom, pid %d, arrives at barrier\n", pid);
	printf("Currently %d Hydrogens and %d Carbons waiting\n", shared->waiting_H + 1, shared->waiting_C);
	fflush(stdout);

	// if enough C and H is waiting, continue past barrier
	if (shared->waiting_H >= 3 
		&& shared->waiting_C >= 1) {
		// release 3 H
		for (int i=0; i < 3; i++) {
			semSignal(semid, SH);
		}
		shared->waiting_H -= 3;
		semSignal(semid, SC); // release 1 C
		shared->waiting_C -= 1;

		fflush(stdout);
		printf("\nHello from %d, 1 CH4 molecule has xed the barrier\n\n", pid);
		fflush(stdout);
		// release lock on mutex
		semSignal(semid, MUTEX);
	} else {
		// not enough C or H is waiting, so wait at barrier
		shared->waiting_H += 1;
		// relaese lock on mutex
		semSignal(semid, MUTEX);
		semWait(semid, SH);
	}
	pthread_exit(NULL);
}

void *start_carbon(void* arg) {
	printf("start_carbon called\n");
	struct common *shared;//pointer to shared data structure

	int semid, shmid;//semaphore memory id, shared memory id

	int pid = getpid();

	//get semaphore memory id
	if ((semid = semget(SEMKEY, NUM_SEMS, 0777)) < 0) {
		perror("semget");
		exit(EXIT_FAILURE);
	}

	//get shared memory id
	if ((shmid = shmget(SHMKEY, 1*K, 0777)) < 0) {
		perror("shmget");
		exit(EXIT_FAILURE);
	}

	//get pointer to shared data structure
	if ((shared = (struct common *)shmat(shmid, 0, 0)) < 0) {
		perror("shmat");
		exit(EXIT_FAILURE);
	}

	// acquire lock on mutex before accessing shared memory
	semWait(semid, MUTEX);

	fflush(stdout);
	printf("Hydrogen atom, pid %d, arrives at barrier\n", pid);
	printf("Currently %d Hydrogens and %d Carbons waiting\n", shared->waiting_H + 1, shared->waiting_C);
	fflush(stdout);

	// if enough C and H is waiting, continue past barrier
	if (shared->waiting_H >= 3 
		&& shared->waiting_C >= 1) {
		// release 3 H
		for (int i=0; i < 3; i++) {
			semSignal(semid, SH);
		}
		shared->waiting_H -= 3;
		semSignal(semid, SC); // release 1 C
		shared->waiting_C -= 1;

		fflush(stdout);
		printf("\nHello from %d, 1 CH4 molecule has xed the barrier\n\n", pid);
		fflush(stdout);
		// release lock on mutex
		semSignal(semid, MUTEX);
	} else {
		// not enough C or H is waiting, so wait at barrier
		shared->waiting_H += 1;
		// relaese lock on mutex
		semSignal(semid, MUTEX);
		semWait(semid, SH);
	}
	pthread_exit(NULL);
}