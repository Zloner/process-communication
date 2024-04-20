#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

using namespace std;

#define MAX_QUEUE_SIZE 9
#define QUEUE_MAX_LENGTH 9

template <class T>
class Queue{
private:
    T* m_data;
    int m_head;
    int m_end;
    int m_size; //加锁
public:
    Queue():m_head(0), m_end(0), m_size(0){
        m_data = (T*)malloc(sizeof(T) * QUEUE_MAX_LENGTH);
        memset(m_data, 0, QUEUE_MAX_LENGTH);
    }
    ~Queue(){
        m_head = 0; 
        m_end = 0; 
        m_size = 0;
        if(m_data != NULL)
            free(m_data);
    }
    int Qsize(){
        //lock
        return m_size;
        //unlock
    }
    bool Qempty(){
        //lock
        return !m_size;
        //unlock
    }
    void Qpush(T x){
        //lock
        if(m_size >= QUEUE_MAX_LENGTH){
            printf("queue is full\n");
            wait
        }
        m_size++;
        //unlock
        m_data[m_end] = x;
        m_end = (m_end + 1) % QUEUE_MAX_LENGTH;
        
    }
    void Qpop(){
        //lock
        if(m_size == 0){
            printf("queue is empty\n");
            wait
        }
        m_size--;
        //unlock
        m_head = (m_head + 1) % QUEUE_MAX_LENGTH;
        
    }
    T Qfront(){
        if(m_size == 0){
            printf("queue is empty\n");
            return NULL;
        }
        return m_data[m_head];
    }
};

Queue<int*> qu;
pthread_cond_t queue_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_push = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_pop = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_size = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_qu_full = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_qu_empty = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_wf = PTHREAD_MUTEX_INITIALIZER;

int* createArr(int th_id){
    int * arr = (int*)malloc(sizeof(int) * 5);

    srand((unsigned)time(NULL)*(th_id+1));
    for(int i = 0; i < 5; i++){
        arr[i] = rand()%300;
    }
    return arr;
}

int randomsecond(int id){
    int time_t = 1;
    srand((unsigned)time(NULL) * id);
    while((rand() & 0xFFFF) < (0.20 * 0xFFFF)){
        time_t++;
    }
    return time_t < 10 ? time_t : 10;
}

void* producer(void* arg){
    int id = (long int)arg;
    printf("thread(producer) %d start.\n", id);

    while(true){
        int* arr = createArr(id);

        //pthread_mutex_lock(&mutex_size);//push
        while(qu.Qsize() >= MAX_QUEUE_SIZE){
            pthread_mutex_lock(&mutex_qu_full);
            pthread_cond_wait(&queue_full, &mutex_qu_full);
            pthread_mutex_unlock(&mutex_qu_full);
        }
        //pthread_mutex_unlock(&mutex_size);

        //pthread_mutex_lock(&mutex_push);
        qu.Qpush(arr);

       // pthread_mutex_unlock(&mutex_size);

        //pthread_mutex_unlock(&mutex_push);


        pthread_cond_signal(&queue_empty);

        printf("thread id:%d create array\n", id);
        int time_t = 1;
        printf("sleep %ds\n", time_t);
        sleep(time_t);
    }   
    /*while(true){
        int* arr = createArr(id);

        if(qu.Qsize() < MAX_QUEUE_SIZE){ 
            pthread_mutex_lock(&mutex_push);
            qu.Qpush(arr);
            pthread_mutex_unlock(&mutex_push);
            pthread_cond_signal(&pcond);
        }

        printf("thread id:%d create array\n", id);
        int time_t = 1; //randomsecond(id);
        printf("sleep %ds\n", time_t);
        sleep(time_t);
    }
    */

    return arg;
}

void* consumer(void * arg){
    int id = (long int)arg;
    printf("thread(consumer) %d start.\n", id);

    while(true){
        int * arr = NULL;

        while(qu.Qsize() <= 0){
            pthread_mutex_lock(&mutex_qu_empty);
            pthread_cond_wait(&queue_empty, &mutex_qu_empty);
            pthread_mutex_unlock(&mutex_qu_empty);
        }

        pthread_mutex_lock(&mutex_pop);
        arr = qu.Qfront();
        qu.Qpop();
        pthread_mutex_unlock(&mutex_pop);

        if(arr != NULL){

            for(int i = 0; i < 100000000; i++){

            }

            sort(arr, arr + 5, greater<int>());
            free(arr);
        }
    }
    /*
    while(true){
        int * arr = NULL;
        pthread_mutex_lock(&mutex_con);
        pthread_cond_wait(&pcond, &mutex_con);
        
        if(!qu.Qempty()){
            pthread_mutex_lock(&mutex_pop);
            arr = qu.Qfront();
            qu.Qpop();
            pthread_mutex_unlock(&mutex_pop);
        }

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
        else{
            printf("queue is empty\n");
        }
        pthread_mutex_unlock(&mutex_con);
    }
    */

    return arg;
}

int main(){
    vector<pthread_t> idt;
    int res = 0;
    FILE* f = fopen("result.txt", "w");
    fclose(f);

    idt.resize(5);
    for(int i = 0; i < 3; i++){
        res = pthread_create(&idt[i], 0, producer, (void*)i);
        if(res == -1){
            perror("pthread create");
            exit(-1);
        }
    }

    for(int i = 3; i < 5; i++){
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