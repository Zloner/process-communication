#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

struct my_msgbuf{
    long mtype;
    char mtext[128];
};

int main(){
    struct my_msgbuf readbuf;
    struct my_msgbuf writebuf = {988, "thank you"};
    int msgid = msgget(0x1234, IPC_CREAT | 0777);
    if(msgid == -1){
        printf("fail to create message queue.\n");
    }

    msgrcv(msgid, &readbuf, sizeof(readbuf.mtext), 888, 0);
    printf("%s\n", readbuf.mtext);
    msgsnd(msgid, &writebuf, strlen(writebuf.mtext), 0);
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}