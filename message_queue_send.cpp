#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>

typedef struct {
    long mtype;
    char mtext[128];
}my_msgbuf;

int main(){
    my_msgbuf writebuf = {888, "hello world"};
    my_msgbuf readbuf;
    int msgid = msgget(0x1234, IPC_CREAT | 0777);
    if(msgid == -1){
        printf("fail to create message queue.\n");
        return 0;
    }

    msgsnd(msgid, &writebuf, strlen(writebuf.mtext), 0);
    msgrcv(msgid, &readbuf, sizeof(readbuf.mtext), 988, 0);
    printf("%s\n", readbuf.mtext);
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}