#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, const char* argv[]){
    if(argc < 2){
        printf("no input path.\n");
        exit(1);
    }

    int ret = access(argv[1], F_OK);
    if(ret == -1){
        int r = mkfifo(argv[1], 0664);
        if(r == -1){perror("mkfifo error\n"); exit(1);}
        printf("the fifo is created successfully\n");
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1){
        perror("open error\n");
        exit(1);
    }

    char buf[512];
    int len = read(fd, buf, sizeof(buf));
    buf[len] = '\0';
    printf("buf=%s,\n len=%d\n", buf, len);
    close(fd);
    return 0;
}
