//
// Created by MZiol on 05/12/2017.
//
#include "working_list/sem.h"

#ifndef SOI_LAB3_LIST_H
#define SOI_LAB3_LIST_H

typedef struct {
    int value;
    unsigned int times_read;
} Data;

Data *make_data(int _value);

typedef struct Node {
    Data data;
    struct Node* next;
}Node;

typedef struct List {
    Node* head;
    Node* tail;
    Semaphore *mutex;
    Semaphore *read_mutex;
    Semaphore *items_in;
    Semaphore *space_left;
    int cur_items;
}List;

List *make_list(int buffer_size);
void push(List* list, Data input, int producer_id);
Data try_pop (List* list, int consumer_id);
void print_list(Node *head);

#endif //SOI_LAB3_LIST_H
