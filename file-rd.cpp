#include <stdio.h>
#include <stdlib.h>

int main(){
    char buffer[512];
    FILE * f_lock = NULL;
    while(f_lock == NULL){
        f_lock = fopen("ready", "r");
    }

    FILE* fr = fopen("file", "r");
    fscanf(fr, "%s", buffer);
    printf("%s\n", buffer);
    return 0;
}