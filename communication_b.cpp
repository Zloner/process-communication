#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

typedef struct {
    long mtype;
    char mtext[128];
    char mtime[21];
}my_msgbuf;

void* write2(void * arg){
    my_msgbuf writebuf2;

    key_t key_2 = ftok(".", 18);
    if(key_2 == -1){
        perror("ftok");
        exit(-1);
    }

    int msgid = msgget(key_2, IPC_CREAT | 0777);
    if(msgid == -1){
        perror("msgget");
        exit(-1);
    }

    char YMD[15] = {0};
    char HMS[10] = {0};
    time_t current_time;
    tm* now_time;

    while(1){
        memset(writebuf2.mtime, 0, sizeof(writebuf2.mtime));

        writebuf2.mtype = 2;
        fgets(writebuf2.mtext, 128, stdin);
        printf("\n");
        
        time(&current_time);
        now_time = localtime(&current_time);
        strftime(YMD, sizeof(YMD), "%F ", now_time);
        strftime(HMS, sizeof(HMS), "%T", now_time);

        strncat(writebuf2.mtime, YMD, 11);
        strncat(writebuf2.mtime, HMS, 8);

        msgsnd(msgid, &writebuf2, sizeof(writebuf2.mtext) + sizeof(writebuf2.mtime), 0);
    }

    return arg;
}

void* read1(void * arg){
    my_msgbuf readbuf1;

    key_t key_1 = ftok(".", 17);
    if(key_1 == -1){
        perror("ftok");
        exit(-1);
    }

    int msgid = msgget(key_1, IPC_CREAT | 0777);
    if(msgid == -1){
        perror("magget");
        exit(-1);
    }
    while(1){
        int res = msgrcv(msgid, &readbuf1, sizeof(readbuf1.mtext) + sizeof(readbuf1.mtime), 1, 0);
        if(res == -1){
            perror("msgrcv");
            exit(-1);
        }
        printf("\033[0\033[1;32mmessage(%s): %s\033[0m\n", readbuf1.mtime, readbuf1.mtext);
    }

    return arg;
}

int main(){
    pthread_t id1, id2;

    int res = pthread_create(&id1, 0, write2, 0);
    if(res == -1){
        printf("thread 1 created error\n");
        exit(-1);
    }

    res = pthread_create(&id2, 0, read1, 0);
    if(res == -1){
        printf("thread 2 created error\n");
        exit(-1);
    }

    pthread_join(id1, 0);
    pthread_join(id2, 0);
}