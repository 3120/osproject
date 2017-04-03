#include "rr.h"

/**
 * Round robin processes requests by simply adding them to the back
 * of the queue.
 */
void rr_enqueue(RCB *request) {
	if (!request) return;

    pthread_mutex_lock(&top_lock);
        queue_enqueue(top_queue, request);
    pthread_mutex_unlock(&top_lock);
}

/**
 * Retrieve the RCB from the front of the queue
 */
RCB* rr_dequeue() {
    pthread_mutex_lock(&top_lock);
        RCB *ready_to_serve = queue_dequeue(top_queue);
    pthread_mutex_unlock(&top_lock);

    return ready_to_serve;
}

/**
 * Send a single block of a requested file to the client.
 */
RCB* rr_serve(RCB *rcb) {
    http_write_to_client(rcb, CHUNK_8KB);
    return rcb;
}