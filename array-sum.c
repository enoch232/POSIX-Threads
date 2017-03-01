#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

#define PRODUCER_COUNT 2 // number of Producer
#define CONSUMER_COUNT 1 // number of Consumer
#define ARRAY_SIZE 20 // number of Default array size

typedef struct {
    int a[ARRAY_SIZE]; // array to store first array
    int b[ARRAY_SIZE]; // array to store second array
    int c[ARRAY_SIZE]; // array to store third array
    int array_index; // store shared index when storing the int element to array
    int global_sum; // store shared global sum
    int counter; // store sem counter
    pthread_mutex_t mutex;          // enforce mutual exclusion while sharing data.
} sem_var_t;

sem_var_t shared_variables; // create shared struct.

void *Producer(void *arg) { // Producer
    int index = (int) arg; // store the producer's identity.

    while (shared_variables.array_index < ARRAY_SIZE && shared_variables.global_sum < 200){ // while global sum is less than 200, and array index is not greater than the array size
      while(shared_variables.counter >= 1); // stall if there is one or more producer exists already. This is to ensure number of producer is only max 2 above number of consumer

      pthread_mutex_lock(&shared_variables.mutex); // lock using the mutex, so that no two producers are working on the same array_index
      shared_variables.c[shared_variables.array_index] = shared_variables.a[shared_variables.array_index] + shared_variables.b[shared_variables.array_index]; //add elements of two different arrays.
      shared_variables.global_sum += shared_variables.c[shared_variables.array_index]; //update the global_sum
      printf("Producer %d has written c[%d] and updated global sum.\n", index, shared_variables.array_index); // notify the user.
      shared_variables.array_index += 1; // increase the array_index, so that it moves through array.
      shared_variables.counter += 1; // increase the number of produced items.
      printf("Number of items available for consumer: %d\n\n", shared_variables.counter); //display the new updated number of produced items.
      pthread_mutex_unlock(&shared_variables.mutex); //unlock so that consumer can jump on this and work.
      sleep(1); //take a break so that other producer/consumer can work.
    }
    return NULL; // end this thread after.
}

void *Consumer(void *arg) { // Consumer
    int index = (int) arg; // store the producer's identity.
    int i; // set the variable here, to use it outside the for loop.
    for (i = 0; i < ARRAY_SIZE; i++) { // count through array size.
      while(shared_variables.counter <= 0 && shared_variables.global_sum < 200); // stall when the global sum is under 200 AND there is no produced items.
      pthread_mutex_lock(&shared_variables.mutex); // lock using the mutex
      printf("Consumer %d has read c[%d]= %d.\n", index, i, shared_variables.c[i]); // notify the user what it read.
      printf("Consumer %d has read global sum = %d\n\n", index, shared_variables.global_sum); // notify the user what the new global sum is now.
      shared_variables.counter -= 1; // indicate that it consumed one produced item.
      pthread_mutex_unlock(&shared_variables.mutex); // unlock it, so that producer can produce more.
      sleep(1); // rest so that producers have time to make more items.
      if (shared_variables.global_sum >= 200){ // if the global_sum is above 200,
        printf("Consumer %d has read previously produced c[%d]= %d.\n", index, i+1, shared_variables.c[i+1]); // display previously produced.
        printf("Consumer %d has read global sum = %d\n\n", index, shared_variables.global_sum); // display new global sum
        printf("Consumer %d has read previously produced c[%d]= %d.\n", index, i+2, shared_variables.c[i+2]); // display another previously produced.
        printf("Consumer %d has read global sum = %d\n\n", index, shared_variables.global_sum); // display new global sum
        break; // break out of the loop so that it doesnt stall or anything.
      }
    }
    return NULL; // end this thread after
}

int main() {
    pthread_t idProducer, idConsumer; //set up
    int index;
    for(int i = 0; i < ARRAY_SIZE; i++) { // create the array a
      shared_variables.a[i] = i;
    }
    for(int i = 0; i < ARRAY_SIZE; i++) { // create the array b
      shared_variables.b[i] = i+1;
    }
    for(int i = 0; i < ARRAY_SIZE; i++) { // create the array c
      shared_variables.c[i] = 0;
    }
    shared_variables.array_index = 0; //just in case, set the default of array_index to zero.
    shared_variables.global_sum = 0; // just in case, set the default of global_sum to zero.
    pthread_mutex_init(&shared_variables.mutex, NULL);
    for (index = 0; index < PRODUCER_COUNT; index++) { // loop through the number of producer
        pthread_create(&idProducer, NULL, Producer, (void*)index); // create new producer
    }
    for(index=0; index<CONSUMER_COUNT; index++) { // loop through the number of consumer
        pthread_create(&idConsumer, NULL, Consumer, (void*)index); // create new consumer
    }

    pthread_exit(NULL); // terminate
}
