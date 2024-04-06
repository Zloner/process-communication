#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

using namespace std;

#define MAX_SIZE 9

sem_t product;
pthread_mutex_t mutex_push = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pop = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_wf = PTHREAD_MUTEX_INITIALIZER;
queue<int*> qu;

int* createArr(int th_id){
    int * arr = (int*)malloc(sizeof(int) * 5);

    srand((unsigned)time(NULL)*(th_id+1));
    for(int i = 0; i < 5; i++){
        arr[i] = rand()%300;
    }
    return arr;
}

void* producer(void * arg){
    int id = (long int)arg;
    printf("thread(producer) %d start.\n", id);
    while(true){
        int * arr = createArr(id);

        pthread_mutex_lock(&mutex_push);
        if(qu.size() < MAX_SIZE){
            qu.push(arr);
        }
        pthread_mutex_unlock(&mutex_push);

        printf("thread id:%d create array\n");
        sleep(1);
        sem_post(&product);
    }
    return arg;
}

void * consumer(void * arg){
    int id = (long int)arg;
    int* arr;
    printf("thread(consumer) %d start.\n", id);
    while(true){
        int * arr = NULL;

        pthread_mutex_lock(&mutex_pop);
        if(!qu.empty()){
            arr = qu.front();
            qu.pop();
        }
        pthread_mutex_unlock(&mutex_pop);

        if(arr != NULL){
            sort(arr, arr + 5, greater<int>());

            FILE* fw = NULL;
            pthread_mutex_lock(&mutex_wf);
            fw = fopen("result.txt", "a");
            for(int i = 0; i < 5; i++){
                fprintf(fw, "%d ", arr[i]);
            }
            fprintf(fw, "\n");
            fclose(fw);
            pthread_mutex_unlock(&mutex_wf);

            printf("thread %d print array: ", id);
            for(int i = 0; i < 5; i++){
                printf("%d ", arr[i]);
            }
            printf("\n");
            free(arr);
        }
    }
    return arg;
}

int main(){
    vector<pthread_t> idt;
    int i, res;
    FILE* f = fopen("result.txt", "w");
    fclose(f);

    sem_init(&product, 0, 0);

    idt.resize(5);
    for(i = 0; i < 2; i++){
        res = pthread_create(&idt[i], 0, producer, (void*)i);
        if(res == -1){
            perror("pthread error");
            exit(-1);
        }
    }
    for(i = 2; i < 5; i++){
        res = pthread_create(&idt[i], 0, consumer, (void*)i);
        if(res == -1){
            perror("pthread error");
            exit(-1);
        }
    }

    for(i = 0; i < 5; i++){
        pthread_join(idt[i], 0);
    }
    return 0;
}