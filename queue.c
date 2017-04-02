#include "queue.h"
#include <stdlib.h>

Queue* queue_init() {
	Queue *new_queue = malloc(sizeof(Queue));

	if (new_queue) {
		new_queue->head = NULL;
	} else {
		perror("Error while allocating memory");
		abort();
	}

	return new_queue;
}

void queue_destroy(Queue *victim) {
	if (!victim) {
		return;
	}

	Node *current = victim->head;
	while (current) {
		Node *next = current->next;
		/* Destroy the RCB */
		free(current);
		current = next;
	}

	free(victim);
}

void queue_enqueue(Queue *queue, RCB *rcb) {
	if (!queue || !rcb) return;

	Node *new_node = node_init(rcb);

	if (queue_is_empty(queue)) {
		queue->head = new_node;
	} else {
		Node *current_end = queue->head;
		while (current_end->next) {
			current_end = current_end->next;
		}
		current_end->next = new_node;
	}
}

bool queue_enqueue_priority(Queue *queue, RCB *rcb) {
	if (!queue || !rcb) return false;

	Node *new = node_init(rcb);

	/* If queue is empty, add new as first element */
	if (queue->head == NULL) {
		queue->head = new;
		return true;
	}
	
	/* If queue is not empty, find the first element which is larger than
	 * the new node's file. Add the new node directly before that element.
	 */
	Node *comparison = queue->head;
	Node *prev = NULL;
	while (comparison != NULL) {
		if (rcb->bytes_unsent >= comparison->rcb->bytes_unsent) {
			prev = comparison;
			comparison = comparison->next;
		} else {
			new->next = comparison;
			if (prev == NULL) {
				queue->head = new;
			} else {
				prev->next = new;
			}
			return true;
		}
	}

	/* If no elements are larger than the new node, add the new node to
	 * the end of the queue.
	 */
	if (comparison == NULL) {
		prev->next = new;
		return true;
	}

	return false;
}

RCB* queue_dequeue(Queue *queue) {
	if (!queue || queue->head == NULL) return NULL;

	Node *pop = queue->head;
	queue->head = queue->head->next;

	RCB *request = pop->rcb;
	pop->rcb = NULL;
    node_destroy(pop);
	return request;
}

bool queue_is_empty(Queue *queue) {
	return queue->head == NULL;
}
