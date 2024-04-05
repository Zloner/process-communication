#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

using namespace std;

#define MAX_QUEUE_SIZE 9

queue<int*> qu;
pthread_cond_t pcond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_push = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_con = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pop = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_wf = PTHREAD_MUTEX_INITIALIZER;

int* createArr(int th_id){
    int * arr = (int*)malloc(sizeof(int) * 5);

    srand((unsigned)time(NULL)*(th_id+1));
    for(int i = 0; i < 5; i++){
        arr[i] = rand()%300;
    }
    return arr;
}

void* producer(void* arg){
    int id = (long int)arg;
    printf("thread(producer) %d start.\n", id);
    while(true){
        while(qu.size() < MAX_QUEUE_SIZE){
            int* arr = createArr(id);
            pthread_mutex_lock(&mutex_push);
            qu.push(arr);
            pthread_mutex_unlock(&mutex_push);
            printf("thread id:%d create array\n", id);
            sleep(1);
            pthread_cond_broadcast(&pcond);
        }
    }

    return arg;
}

void* consumer(void * arg){
    int id = (long int)arg;
    printf("thread(consumer) %d start.\n", id);
    while(true){
        pthread_mutex_lock(&mutex_con);
        pthread_cond_wait(&pcond, &mutex_con);
        if(!qu.empty()){

            pthread_mutex_lock(&mutex_pop);
            int* arr = qu.front();
            qu.pop();
            pthread_mutex_unlock(&mutex_pop);

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
        pthread_mutex_unlock(&mutex_con);
    }
    return arg;
}

int main(){
    vector<pthread_t> idt;
    int res = 0;
    FILE* f = fopen("result.txt", "w");
    fclose(f);

    idt.resize(5);
    for(int i = 0; i < 2; i++){
        res = pthread_create(&idt[i], 0, producer, (void*)i);
        if(res == -1){
            perror("pthread create");
            exit(-1);
        }
    }

    for(int i = 2; i < 5; i++){
        res = pthread_create(&idt[i], 0, consumer, (void*)i);
        if(res == -1){
            perror("pthread create");
            exit(-1);
        }
    }

    for(int i = 0; i < 5; i++){
        pthread_join(idt[i], 0);
    }

    return 0;
}