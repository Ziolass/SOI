//
// Created by MZiol on 05/12/2017.
//
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "list.h"

#define MIN_ITEMS_FOR_READ 3
#define MIN_NUM_ITEM_READ 2

Data *make_data(int _value){
    Data *data = (Data *) malloc(sizeof(Data));
    data->value = _value;
    data->times_read = 0;
    return data;
}

List *make_list(int buffer_size){
    List* list = malloc(1* sizeof(List));
    if( list == NULL){
        perror_exit("List creation failed");
    }
    list->head = list->tail = NULL;
    list->mutex = make_semaphore(1);
    list->items_in = make_semaphore(0);
    list->space_left = make_semaphore(buffer_size - 1);
    list->cur_items = 0;
    list->read_mutex = make_semaphore(0);
    return list;

}

void push(List* list, Data input, int producer_id){

    Node* node = malloc(1*sizeof(*node));

    if (node == NULL){
        perror_exit("Error while initialising new node");
    }

    node->data = input;
    node->next = NULL;

    printf("\tProducer %d waits for space_left...\n", producer_id);
    semaphore_wait(list->space_left);
    printf("\tProducer %d obtained space_left.\n", producer_id);

    printf("\tProducer %d waits for mutex...\n", producer_id);
    semaphore_wait(list->mutex);
    printf("\tProducer %d obtained mutex.\n", producer_id);


    if( list->head == NULL&& list->tail == NULL)
    {
        // list empty, first = last
        list->head = list->tail = node;
    }
    else if( NULL == list->head || NULL == list->tail )
    {
        free(node);
        perror_exit("Sth wrong with the list, list or tail set to null");
    }
    else
    {
        // adding element to tail
        list->tail->next = node;
        list->tail = node;
    }
    if (++list->cur_items >= MIN_ITEMS_FOR_READ){
        semaphore_signal(list->read_mutex);
    }

    semaphore_signal(list->mutex);
    printf("\tProducer %d released  mutex.\n", producer_id);

    semaphore_signal(list->items_in);
    printf("\tProducer %d released items_in.\n", producer_id);

    printf("Producer %d added item %d\n", producer_id, input.value);
}

Data try_pop (List* list, int consumer_id) {

    Node* current = NULL;
    Node* p = NULL;
    Data returnable;

    printf("\tConsumer %d waits for items_in...\n", consumer_id);
    semaphore_wait(list->items_in);
    printf("\tConsumer %d obtained items_in.\n", consumer_id);

    printf("\tConsumer %d waits for read_mutex...\n", consumer_id);
    semaphore_wait(list->read_mutex);
    printf("\tConsumer %d obtained read_mutex.\n", consumer_id);

    printf("\tConsumer %d waits for mutex...\n", consumer_id);
    semaphore_wait(list->mutex);
    printf("\tConsumer %d obtained mutex.\n", consumer_id);


    if( NULL == list->head && NULL == list->tail )
    {
        printf("Well, List is empty\n");
    }
    else if( NULL == list->head || NULL == list->tail )
    {
        printf("There is something seriously wrong with your list\n");
        printf("One of the head/tail is empty while other is not \n");
    }

    current = list->head;
    p = current->next;
    returnable = list->head->data;

    if (++current->data.times_read >= MIN_NUM_ITEM_READ){
        printf("Consumer %d: read %d and removed from buffer\n", consumer_id, returnable.value);
        free(current);
        list->head = p;
        if( NULL == list->head )  list->tail = list->head;   /* The element tail was pointing to is free(), so we need an update */
        semaphore_signal(list->space_left);
        printf("\tConsumer %d released spaces_left.\n", consumer_id);

    }
    else{
        printf("Consumer %d: read %d but did not removed from buffer. Inner counter %d < %d\n",
               consumer_id, returnable.value, current->data.times_read, MIN_NUM_ITEM_READ);

        semaphore_signal(list->items_in);
        printf("\tConsumer %d released items_in.\n", consumer_id);
        semaphore_signal(list->read_mutex);
        printf("\tConsumer %d released read_mutex.\n", consumer_id);

    }
    semaphore_signal(list->mutex);
    printf("\tConsumer %d released mutex.\n", consumer_id);

    print_list(list->head);

    return returnable;
}

void print_list(Node *head){
    Node *current = head;

    printf("\n##### CURRENT BUFFER STATUS ######\n");
    while (current != NULL) {
        printf("%d ", current->data.value);
        current = current->next;
    }
    printf("\n##################################\n");
}
