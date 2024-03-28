#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short * array;
    struct seminfo *__buf;
};

static int SemPV(int semid, int which, int _op){
    struct sembuf sf;
    sf.sem_flg = SEM_UNDO;
    sf.sem_num = which;
    sf.sem_op = _op;
    return semop(semid, &sf, 1);
}

void * read2(void * arg){
    key_t key_2 = ftok(".", 19);
    if(key_2 == -1){
        perror("ftok");
    }


    int shmid_2 = shmget(key_2, 512, IPC_CREAT|0666);
    if(shmid_2 < 0){
        perror("shmget");
        exit(-1);
    }
    char * shmadd = (char*)shmat(shmid_2, NULL, 0);
    if(shmadd < 0){
        perror("shmat");
        exit(-1);
    }

    int semid_2 = semget(key_2, 1, IPC_CREAT|0666);
    if(semid_2 == -1){
        perror("semget");
        exit(-1);
    }
    
    while(1){
        SemPV(semid_2, 0, -1);

        printf("%s", shmadd);
        fflush(stdout);
        memset(shmadd, 0, 512);

        SemPV(semid_2, 0, 1);
    }
    return arg;
}

void * write1(void * arg){
    key_t key_1 = ftok(".", 18);
    if(key_1 == -1){
        perror("ftok");
    }

    int shmid_1 = shmget(key_1, 512, IPC_CREAT|0666);
    if(shmid_1 < 0){
        perror("shmget");
        exit(-1);
    }
    char * shmadd = (char*)shmat(shmid_1, NULL, 0);
    if(shmadd < 0){
        perror("shmat");
        exit(-1);
    }

    int semid_1 = semget(key_1, 1, IPC_CREAT|0666);
    if(semid_1 == -1){
        perror("semget");
        exit(-1);
    }
    union semun semval;
    semval.val = 1;
    if(semctl(semid_1, 0, SETVAL, semval) < 0){
        perror("init");
        exit(-1);
    }
    
    while(1){
        SemPV(semid_1, 0, -1);

        fgets(shmadd, 512, stdin);

        SemPV(semid_1, 0, 1);
    }
    return arg;
}

int main(){
    pthread_t idt1;
    pthread_t idt2;

    int res = pthread_create(&idt1, 0, read2, 0);
    if(res == -1){
        printf("create thread 1 fail\n");
        exit(-1);
    }

    res = pthread_create(&idt2, 0, write1, 0);
    if(res == -1){
        printf("create thread 2 fail\n");
        exit(-1);
    }

    pthread_join(idt2, 0);
    pthread_join(idt1, 0);

    return 0;
}
