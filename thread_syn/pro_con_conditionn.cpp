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
#define QUEUE_MAX_LENGTH 20

template <class T>
class Queue{
private:
    T* m_data;
    int m_head;
    int m_end;
    int m_size;
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
        return m_size;
    }
    bool Qempty(){
        return !m_size;
    }
    void Qpush(T x){
        if(m_size >= QUEUE_MAX_LENGTH){
            printf("queue is full\n");
            return;
        }
        m_data[m_end] = x;
        m_end = (m_end + 1) % QUEUE_MAX_LENGTH;
    }
    void Qpop(){
        if(m_size == 0){
            printf("queue is empty\n");
            return;
        }
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

int randomsecond(){
    int time_v = 1;
    srand((unsigned int)time(NULL));
    while(rand() * 0xffff > 0.5 * 0xffff){
        time_v++;
    }
    return time_v < 10 ? time_v : 10;
}

void* producer(void* arg){
    int id = (long int)arg;
    printf("thread(producer) %d start.\n", id);
    while(true){
        int* arr = createArr(id);

        pthread_mutex_lock(&mutex_push);
        if(qu.Qsize() < MAX_QUEUE_SIZE){
            qu.Qpush(arr);
        }
        pthread_mutex_unlock(&mutex_push);
        
        printf("thread id:%d create array\n", id);
        int time_v = randomsecond();
        sleep(time_v);
        pthread_cond_broadcast(&pcond);
    }

    return arg;
}

void* consumer(void * arg){
    int id = (long int)arg;
    printf("thread(consumer) %d start.\n", id);
    while(true){
        int *arr = NULL;
        pthread_mutex_lock(&mutex_con);
        pthread_cond_wait(&pcond, &mutex_con);

        pthread_mutex_lock(&mutex_pop);
        if(!qu.Qempty()){
            int* arr = qu.Qfront();
            qu.Qpop();
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