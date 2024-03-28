#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *arry;
    struct seminfo *__buf;
};

static int SemPV(int semid, int which, int _op){
    struct sembuf sf;
    sf.sem_num = which;
    sf.sem_op = _op;
    sf.sem_flg = 0;
    return semop(semid, &sf, 1);
}

int main(){
    key_t key = ftok(".", 2012);
    if(key < 0){
        perror("ftok");
    }

    int semid = semget(IPC_PRIVATE, 1, IPC_CREAT|0666);
    if(semid == -1){
        printf("fail to create semaphore.\n");
        return 1;
    }
    printf("create %d sem success.\n", semid);

    union semun semval;
    semval.val = 1;
    if(semctl(semid, 0, SETVAL, semval) < 0){
        perror("semctl");
        return 1;
    }

    pid_t id = fork();

    if(id == 0){
        while(1){
            if(SemPV(semid, 0, -1) < 0){
                printf("p");
                return 1;
            }
            sleep(2);
            printf("A");
            fflush(stdout);
            sleep(2);
            printf("A");
            fflush(stdout);
            sleep(2);
            if(SemPV(semid, 0, 1) < 0){
                perror("v");
                return 1;
            }
        }
    }
    else{
        while(1){
            if(SemPV(semid, 0, -1) < 0){
                printf("p");
                return 1;
            }
            sleep(2);
            printf("B");
            fflush(stdout);
            sleep(2);
            printf("B");
            fflush(stdout);
            sleep(2);
            if(SemPV(semid, 0, 1) < 0){
                perror("v");
                return 1;
            }
        }
        if(waitpid(id, NULL, 0) < 0){
            perror("waitpid");
            return 1;
        }
    }
    if(semctl(semid, 0, IPC_RMID) > 0){
        return 0;
    }
    else{
        perror("destroy");
        return 1;
    }
    return 0;
}
