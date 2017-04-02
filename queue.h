#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "rcb.h"
#include "node.h"

typedef struct {
	Node *head;
} Queue;

Queue* queue_init();
void queue_destroy(Queue *queue);
void queue_enqueue(Queue *queue, RCB *rcb);
bool queue_enqueue_priority(Queue *queue, RCB *rcb);
RCB* queue_dequeue(Queue *queue);
bool queue_is_empty(Queue *queue);

#endif
