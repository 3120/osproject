#include "mlfb.h"

/**
 * Place an RCB in an appropriate queue based on how many times
 * the request has been processed without completion.
 */
void mlfb_enqueue(RCB *rcb) {
    if (!rcb) return;

    switch (rcb->chunks_served) {
        case 0: queue_enqueue(top_queue, rcb); break;
        case 1: queue_enqueue(middle_queue, rcb); break;
        default: queue_enqueue(bottom_queue, rcb); break;
    }
}

/**
 * Grab an RCB from one of the three queues, prioritizing those
 * requests that have been processed fewer times.
 */
RCB* mlfb_dequeue() {
    RCB *next_in_line = NULL;

    /* Pull from high-priority queue if possible */
    pthread_mutex_lock(&top_lock);
        if (!queue_is_empty(top_queue)) next_in_line = queue_dequeue(top_queue);
    pthread_mutex_unlock(&top_lock);
    if (next_in_line) return next_in_line;

    /* Pull from mid-priority queue if top is empty */
    pthread_mutex_lock(&middle_lock);
        if (!queue_is_empty(middle_queue)) next_in_line = queue_dequeue(middle_queue);
    pthread_mutex_unlock(&middle_lock);
    if (next_in_line) return next_in_line;

    /* Otherwise look in the round-robin queue */
    pthread_mutex_lock(&bottom_lock);
        if (!queue_is_empty(bottom_queue)) next_in_line = queue_dequeue(bottom_queue);
    pthread_mutex_unlock(&bottom_lock);
    return next_in_line;
}

/**
 * Sends a portion of the requested file, based on the queue the request
 * block currently resides in.
 */
RCB* mlfb_serve(RCB *rcb) {
    /* Determine size to send from file priority */
    int send_length = (rcb->chunks_served == 0) ? CHUNK_8KB : CHUNK_64KB;
    http_write_to_client(rcb, send_length);

    return rcb;
}