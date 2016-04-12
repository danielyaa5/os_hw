#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define SEMKEY 77
#define SHMKEY 77

#define NUM_SEMS	3
#define MUTEX		0
#define SH	1
#define SC	2

#define	NUM_C	5
#define	NUM_H	20

#define K 1024//used for shared memory creation

enum MoleculeName { Carbon, Hydrogen };

//used to initialize semaphores
union semun {
	unsigned short *array;
};

//shared data structure among processes
struct common {
	int waiting_C;
	int waiting_H;
};

int retVal;

void semWait(int semid, int semaphore) {
	struct sembuf psembuf;

	psembuf.sem_op = -1;
	psembuf.sem_flg = 0;
	psembuf.sem_num = semaphore;
	semop(semid,&psembuf,1);
	// if ((retVal = semop(semid,&psembuf,1)) != 0) {
	// 	perror("semop");
	// 	exit(EXIT_FAILURE);
	// }
	
	return;
}

void semSignal(int semid, int semaphore) {
	struct sembuf vsembuf;

	vsembuf.sem_op = 1;
	vsembuf.sem_flg = 0;
	vsembuf.sem_num = semaphore;
	semop(semid,&vsembuf,1);
	// if ((retVal = semop(semid,&vsembuf,1)) != 0) {
	// 	perror("semop");
	// 	exit(EXIT_FAILURE);
	// }
	
	return;
}
