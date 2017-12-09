//
// Created by MZiol on 05/12/2017.
//

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

#define RANDOM_MAX 200
// UTILITY CODE

void perror_exit(char *s)
{
    perror(s);
    exit(-1);
}

void *check_malloc(int size)
{
    void *p = malloc(size);
    if (p == NULL) perror_exit("malloc failed");
    return p;
}

unsigned int random_milisecond(){
    unsigned int r = 50+(unsigned int )rand() % RANDOM_MAX;
    return 1000*r;
}
