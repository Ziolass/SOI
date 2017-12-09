//
// Created by MZiol on 05/12/2017.
//

#ifndef SOI_LAB3_SEM_H
#define SOI_LAB3_SEM_H
#include <semaphore.h>
typedef sem_t Semaphore;

Semaphore *make_semaphore(int value);
void semaphore_wait(Semaphore *sem);
void semaphore_signal(Semaphore *sem);

#endif //SOI_LAB3_SEM_H
