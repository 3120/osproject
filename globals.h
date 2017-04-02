#ifndef GLOBALS_H
#define GLOBALS_H

#include "queue.h"
#include <pthread.h>
#include <semaphore.h>

Queue *top_queue, *middle_queue, *bottom_queue, *work_queue;

pthread_mutex_t work_lock;
pthread_mutex_t top_lock;
pthread_mutex_t middle_lock;
pthread_mutex_t bottom_lock;

sem_t permission_to_queue;

void (*scheduler_enqueue)();
void (*scheduler_dequeue)();
void (*scheduler_serve)(RCB*);

#endif