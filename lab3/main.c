/* Example code for Think OS.

Copyright 2015 Allen Downey
License: Creative Commons Attribution-ShareAlike 3.0

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#include "utils.h"
#include "sem.h"
#include "list.h"

// Lab3 specific values
#define NUM_PRODUCERS 1
#define NUM_CONSUMERS 3
#define BUFFER_SIZE 9


#define NUM_ITEMS 127
#define ADDITIONAL_PROD_OFFSET 0
#define ADDITIONAL_CONS_OFFSET 0

// SHARED

typedef struct {
    List *list;
} Shared;

Shared *make_shared()
{
    Shared *shared = check_malloc(sizeof(Shared));
    shared->list = make_list(BUFFER_SIZE);
    return shared;
}

// THREAD

pthread_t make_thread(void *(*entry)(void *), Shared *shared)
{
    int ret;
    pthread_t thread;

    ret = pthread_create(&thread, NULL, entry, (void *) shared);
    if (ret != 0) {
        perror_exit("pthread_create failed\n");
    }
    return thread;
}

void join_thread(pthread_t thread)
{
    int ret = pthread_join(thread, NULL);
    if (ret == -1) {
        perror_exit("pthread_join failed\n");
    }
}

// PRODUCER-CONSUMER for queue

void *queue_producer_entry(void *arg)
{
    int i, id;
    static int static_id = 1;
    Data *input;
    Shared *shared = (Shared *) arg;
    id = static_id;
    static_id++;
    for (i=1; i<NUM_ITEMS; i++) {
        usleep(random_milisecond() + 1000*ADDITIONAL_PROD_OFFSET);
        input = make_data(i);
        // printf("Producer %d: adding item %d\n",id , i);
        push(shared->list, *input, id);
    }
    pthread_exit(NULL);
}

void *queue_consumer_entry(void *arg)
{
    int i, id;
    Data result;
    static int static_id = 1;
    Shared *shared = (Shared *) arg;
    id = static_id;
    static_id++;
    for (i=0; i<NUM_ITEMS; i++) {
        usleep(random_milisecond()+1000*ADDITIONAL_CONS_OFFSET);
        result = try_pop(shared->list, id);
        // printf("Consumer %d: read %d\n", id, result.value);
    }
    pthread_exit(NULL);
}

int main()
{
    int i, num_processes=NUM_CONSUMERS+NUM_PRODUCERS;
    pthread_t child[num_processes];

    Shared *shared = make_shared();

    child[0] = make_thread(queue_producer_entry, shared);
    //child[1] = make_thread(queue_consumer_entry, shared);
    //child[2] = make_thread(queue_consumer_entry, shared);
    child[3] = make_thread(queue_consumer_entry, shared);

    for (i=0; i<num_processes; i++) {
         join_thread(child[i]);
    }




    return 0;
}