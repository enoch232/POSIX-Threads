#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include </usr/include/semaphore.h>

#define PRODUCER_COUNT 2
#define CONSUMER_COUNT 1
#define ARRAY_SIZE 20

typedef struct {
    int a[ARRAY_SIZE];
    int b[ARRAY_SIZE];
    int c[ARRAY_SIZE];
    int array_index;
    int global_sum;
    sem_t count;           /* keep track of the number of full spots */
    pthread_mutex_t mutex;          /* enforce mutual exclusion to shared data */
} sem_var_t;

sem_var_t shared_variables;


void *Producer(void *arg) {
    int index = (int) arg;
    while (shared_variables.array_index < 20){
      sem_wait(&shared_variables.count);
      pthread_mutex_lock(&shared_variables.mutex);
      shared_variables.c[shared_variables.array_index] = shared_variables.a[shared_variables.array_index] + shared_variables.b[shared_variables.array_index];
      printf("Producer %d has written c[%d] and updated global sum.\n", index, shared_variables.array_index);
      shared_variables.array_index += 1;
      pthread_mutex_unlock(&shared_variables.mutex);
      sem_post(&shared_variables.count);
      // sleep(1);
    }
    return NULL;
}

void *Consumer(void *arg) {
    int index = (int) arg;
    for (int i = 0; i < 20; i++) {
      sem_wait(&shared_variables.count);
      pthread_mutex_lock(&shared_variables.mutex);
      printf("Consumer %d has read c[%d]= %d.\n", index, i, shared_variables.c[i]);
      pthread_mutex_unlock(&shared_variables.mutex);
      // printf("Consumer created! %d\n", shared_variables.c[i]);

    }
    return NULL;
}

int main() {
    pthread_t idP, idC;
    int index;
    sem_init(&shared_variables.count, 0, 0);

    for(int i = 0; i < 20; i++) {
      shared_variables.a[i] = i;
    }
    for(int i = 0; i < 20; i++) {
      shared_variables.b[i] = i+1;
    }
    for(int i = 0; i < 20; i++) {
      shared_variables.c[i] = 0;
    }
    shared_variables.array_index = 0;
    shared_variables.global_sum = 0;

    for (index = 0; index < PRODUCER_COUNT; index++) {
        /* Create a new producer */
        pthread_create(&idP, NULL, Producer, (void*)index);
    }
    /*create a new Consumer*/
    for(index=0; index<CONSUMER_COUNT; index++) {
        pthread_create(&idC, NULL, Consumer, (void*)index);
    }

    pthread_exit(NULL);
}
