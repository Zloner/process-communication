#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>

using namespace std;

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

int main(){
    pid_t pid;
    key_t key;
    int semid;

    key = ftok(".", 64);
    semid = semget(key, )

    pid = fork();
    if(pid == -1){
        perror("fork");
        exit(-1);
    }
    else if(pid == 0){

    }
    else{

    }
}