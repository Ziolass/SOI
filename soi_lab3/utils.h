//
// Created by MZiol on 05/12/2017.
//

#ifndef SOI_LAB3_UTILS_H
#define SOI_LAB3_UTILS_H


typedef pthread_mutex_t Mutex;
typedef pthread_cond_t Cond;

void perror_exit(char *s);
void *check_malloc(int size);
unsigned int random_milisecond();

#endif //SOI_LAB3_UTILS_H
