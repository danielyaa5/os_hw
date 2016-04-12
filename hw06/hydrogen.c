#include "main.h"

struct common *shared;//pointer to shared data structure

void printVariables(void);

int main(int argc, char const *argv[]) {
	int semid, shmid, sems, semw;//semaphore memory id, shared memory id

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
	if ((sems = semWait(semid, MUTEX)) == 0) {
		perror("semWait");
		exit(EXIT_FAILURE);
	}


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
		if ((sems = semSignal(semid, SC)) == 0) {
			perror("semSignal");
			exit(EXIT_FAILURE);
		};
		shared->waiting_C -= 1;

		fflush(stdout);
		printf("\nHello from %d, 1 CH4 molecule has xed the barrier\n\n", pid);
		fflush(stdout);
		// release lock on mutex
		if ((sems = semSignal(semid, MUTEX)) == 0) {
			perror("semSignal");
			exit(EXIT_FAILURE);
		};
		pthread_exit(NULL);
	} else {
		// not enough C or H is waiting, so wait at barrier
		shared->waiting_H += 1;
		// relaese lock on mutex
		if ((sems = semSignal(semid, MUTEX)) == 0) {
			perror("semSignal");
			exit(EXIT_FAILURE);
		};

		if ((sems = semWait(semid, SH)) == 0) {
			perror("semWait");
			exit(EXIT_FAILURE);
		}
	}
}