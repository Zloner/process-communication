#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

void * write2(void *arg){
    char buffer[128];
    int ret, fd;
    
    ret = access("./fifo2", F_OK);
    if(ret == -1){
        int r = mkfifo("./fifo2", 0777);
        if(r == -1){
            perror("mkfifo");
            exit(-1);
        }
    }

    while(1){
        fd = open("./fifo2", O_WRONLY);
        if(fd == -1){
            perror("open");
            exit(-1);
        }
        fgets(buffer, 128, stdin);
        write(fd, buffer, 128);
        close(fd);
    }
    return arg;
}

void * read1(void *arg){
    int ret, fd, len;
    char buffer[128];

    while(access("./fifo1", F_OK) == -1){};

    while(1){
        fd = open("./fifo1", O_RDONLY);
        if(fd == -1){
            perror("open");
            exit(-1);
        }
        len = read(fd, buffer, 128);
        buffer[len] = '\0';
        printf("%s", buffer);
        memset(buffer, 0, 128);
    }

    return arg;
}

int main(){
    pthread_t idt1, idt2;
    int res;

    res = pthread_create(&idt1, 0, write2, 0);
    if(res == -1){
        printf("create thread1 fail\n");
        exit(-1);
    }

    res = pthread_create(&idt2, 0, read1, 0);
    if(res == -1){
        printf("create thread2 fail\n");
        exit(-1);
    }

    pthread_join(idt1, 0);
    pthread_join(idt2, 0);

    return 0;
}