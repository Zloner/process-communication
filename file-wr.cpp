#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    FILE * fw = fopen("file", "w");
    fprintf(fw, "%s", "hello_world");
    fflush(fw);
    sleep(10);

    FILE * f_lock = fopen("ready", "w");
    return 0;
}