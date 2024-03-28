#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <pthread.h>

union semun{
    int val;
    struct semid_ds * buf;
    unsigned short * array;
    struct seminfo * __buf;
};

static int SemPV(int semid, int which, int _op){
    struct sembuf sf;
    sf.sem_flg = SEM_UNDO;
    sf.sem_num = which;
    sf.sem_op = _op;
    return semop(semid, &sf, 1);
}

//extern int turn = 1;
//extern bool flag[2] = {false, false};

void * write1(void * arg){
    key_t key_1;
    int semid_1;
    union semun semval;
    char buffer[128];

    key_1 = ftok(".", 20);
    if(key_1 == -1){
        perror("ftok");
        exit(-1);
    }
    semid_1 = semget(key_1, 1, IPC_CREAT|0666);
    if(semid_1 < 0){
        perror("semget");
        exit(-1);
    }

    semval.val = 1;
    if(semctl(semid_1, 0, SETVAL, semval) < 0){
        perror("init");
        exit(-1);
    }

    while(1){
        SemPV(semid_1, 0, -1);
        
        FILE * fw = fopen("file1", "w");
        fgets(buffer, 128, stdin);
        fprintf(fw, "%s", buffer);
        fclose(fw);

        SemPV(semid_1, 0, 1);
    }
    /*
    while(1){
        flag[0] = true;
        turn = 1;

        while(turn == 1 && flag[1] == true);

        FILE * fw = fopen("file1", "w");
        fgets(buffer, 128, stdin);
        fprintf(fw, "%s", buffer);
        fclose(fw);

        SemPV(semid_1, 0, 1);
        flag[0] = false;
    }
    */
    return arg;
}

void * read2(void * arg){
    key_t key_2;
    int semid_2;
    union semun semval;
    char buffer[128];

    key_2 = ftok(".", 21);
    if(key_2 == -1){
        perror("ftok");
        exit(-1);
    }
    semid_2 = semget(key_2, 1, IPC_CREAT|0666);
    if(semid_2 < 0){
        perror("semget");
        exit(-1);
    }

    semval.val = 0;
    if(semctl(semid_2, 0, SETVAL, semval) < 0){
        perror("init");
        exit(-1);
    }
    
    while(1){
        SemPV(semid_2, 0, -1);

        FILE* fr = fopen("file2", "r");
        if(fr == NULL){
            perror("file2 read");
            exit(-1);
        }
        fgets(buffer, sizeof(buffer), fr);
        printf("%s", buffer);
        memset(buffer, 0, 128);
        fclose(fr);

        FILE * empty = fopen("file2", "w");
        fclose(empty);
        
        SemPV(semid_2, 0, 1);
    }
    /*
    while(1){
        flag[1] = true;
        turn = 0;
        while(turn == 0 && flag[0] == true);

        FILE* fr = fopen("file2", "r");
        if(fr == NULL){
            perror("file2 read");
            exit(-1);
        }
        SemPV(semid_2, 0, -1);

        fgets(buffer, sizeof(buffer), fr);
        printf("%s", buffer);
        ftruncate(fileno(fr), 0);
        fclose(fr);

        flag[1] = false;
    }
    */

    return arg;
}

int main(){
    pthread_t idt1;
    pthread_t idt2;

    int res = pthread_create(&idt1, 0, write1, 0);
    if(res == -1){
        printf("create thread 1 fail\n");
        return -1;
    }

    res = pthread_create(&idt2, 0, read2, 0);
    if(res == -1){
        printf("create thread 2 fail\n");
        return -1;
    }

    pthread_join(idt1, 0);
    pthread_join(idt2, 0);

    return 0;
}
