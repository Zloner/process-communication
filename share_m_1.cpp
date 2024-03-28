#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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

    char* addr = (char*)shmat(shmid, NULL, 0);
    printf("%p", addr);
    if(addr == (char*)-1){
        perror("shmat");
        return 1;
    }

    memset(addr, 0, 4096);
    fgets(addr, 64, stdin);
    return 0;
}