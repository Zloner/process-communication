#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, const char * argv[]){
    if(argc < 2){
        printf("No input path.\n");
        exit(1);
    }

    int ret = access(argv[1], F_OK);
    if(ret == -1){
        int r = mkfifo(argv[1], 0777);

        if(r == -1){printf("make fifo erro.\n"); exit(1);}
        printf("fifo is created successfully\n");
    }

    int fd = open(argv[1], O_WRONLY);
    if(fd == -1){
        perror("open error\n");
        exit(1);
    }

    char* buffer = "hello world";
    int len = write(fd, buffer, strlen(buffer) + 1);
    close(fd);
    return 0;
}