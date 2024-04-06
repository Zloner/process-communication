#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>

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

struct TNode{
    int data;
    int offset;
    long int lchild;
    long int rchild;
};
typedef struct TNode TNode;
typedef struct TNode* Tree;

long int createtree(int & offset, Tree root, int * arr, int index){
    if(index > arr[0]){
        return -1;
    }

    offset += 1;
    root->offset = offset;
    root->data = arr[index];
    root->lchild = createtree(offset, root+(offset+1-root->offset), arr, index*2+1);
    root->rchild = createtree(offset, root+(offset+1-root->offset), arr, index*2+2);
    return root->offset;
}

//preorder
void showtree(Tree root, Tree tree){

    printf("%d ", root->data);
    if(root->lchild != -1){
        showtree(tree+root->lchild, tree);
    }
    if(root->rchild != -1){
        showtree(tree+root->rchild, tree);
    }
}

int main(){
    pid_t pid;
    key_t key;
    int semid1, semid2, shmid;
    union semun semval;

    //create a semaphore, initial value = 0
    key = ftok(".", 64);
    semid1 = semget(key + 1, 1, IPC_CREAT|0666);
    if(semid1 == -1){
        perror("semget");
        exit(-1);
    }
    semid2 = semget(key + 2, 1, IPC_CREAT|0666);
    if(semid2 == -1){
        perror("semget");
        exit(-1);
    }

    semval.val = 0;
    if(semctl(semid1, 0, SETVAL, semval) < 0){
        perror("init");
        exit(-1);
    }
    if(semctl(semid2, 0, SETVAL, semval) < 0){
        perror("init");
        exit(-1);
    }

    //create shared memory
    shmid = shmget(key, 512 , IPC_CREAT|0666);
    if(shmid < 0){
        perror("shmget");
        exit(-1);
    }

    pid = fork();
    if(pid == -1){
        perror("fork");
        exit(-1);
    }
    //child
    else if(pid == 0){
        //recieve task

        SemPV(semid1, 0, -1);

        int shmid2 = shmget(key+1, 512, IPC_CREAT|0666);
        void * shmadd2 = shmat(shmid2, 0, 0);

        void * shmadd = shmat(shmid, 0, 0);
        printf("shmadd1:%p, shmadd2:%p\n", shmadd, shmadd2);

        int * arr = (int*)shmadd;
        int len = *arr;

        for(int i = 1; i <= len; i++){
            printf("%d\n", arr[i]);
        }
        shmadd += (len + 1) * sizeof(int);

        int offset = -1;
        Tree tree = (Tree)shmadd;
        createtree(offset, tree, arr, 0);
    
        printf("create tree success\n");
        
        SemPV(semid2, 0, 1);
    }
    //father
    else{
        void * shmadd = shmat(shmid, 0, 0);
        printf("shmadd1:%p\n", shmadd);
        memset(shmadd, 0, 512);
        
        int len = 9;
        int * arr = (int*)shmadd;
        arr[0] = len;
        for(int i = 1; i <= len; i++){
            arr[i] = i;
        }
        //release task
        SemPV(semid1, 0, 1);
        SemPV(semid2, 0, -1);
        shmadd+=(len+1) * sizeof(int);
        
        Tree tree = (Tree)shmadd;
        showtree(tree, tree);
        printf("\n");
    }
}