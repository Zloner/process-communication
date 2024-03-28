#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(){
    key_t key = ftok(".", 2015);
    if(key == -1){
        perror("ftok");
    }

    int shmid = shmget(key, 512, IPC_CREAT|0666);
    if(shmid < 0){
        perror("shmget");
        return 1;
    }

    char * shmadd = (char*)shmat(shmid, NULL, 0);
    printf("%p", shmadd);
    if(shmadd < 0){
        perror("shmat");
        return 1;
    }

    printf("data = [%s]\n", shmadd);

    int ret = shmdt(shmadd);
    if(ret < 0){
        perror("shmat");
        return 1;
    }

    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}