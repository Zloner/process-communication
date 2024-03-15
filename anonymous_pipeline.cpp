#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    int fd[2], pid, line;
    char buffer[512];

    if(pipe(fd) == -1){
        perror("create pipe failed\n");
        return 1;
    }

    pid = vfork();
    if(pid < 0){
        perror("can't create a new process.\n");
        return 1;
    }
    else if(pid == 0){
        close(fd[0]);
        printf("child process send message.\n");
        write(fd[1], "Hello world\n", 12);
    }
    else{
        close(fd[1]);
        printf("parent process receive message is:\n");
        line = read(fd[0], buffer, 100);
        write(STDOUT_FILENO, buffer, line);
        printf("\n");
        //Wait for the child process to terminate
        wait(NULL);
        _exit(0);
    }

}