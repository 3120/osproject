#include "sjf.h"

/**
 * At the beginning of every cycle, add any new requests to the
 * queue in order of filesize (ascending).
 */
void sjf_enqueue(RCB *rcb) {
    if (!rcb) return;

    pthread_mutex_lock(&top_lock);
        queue_enqueue_priority(top_queue, rcb);
    pthread_mutex_unlock(&top_lock);
}

/**
 * Grab the RCB at the front of the only queue.
 */
RCB* sjf_dequeue() {
    pthread_mutex_lock(&top_lock);
        RCB *ready_to_serve = queue_dequeue(top_queue);
    pthread_mutex_unlock(&top_lock);

    return ready_to_serve;
}

/**
 * Loops over a file, sending it in 8KB chunks until the
 * client has received it in its entirety. We can spot
 */
RCB* sjf_serve(RCB *rcb) {
    while (!rcb_completed(rcb)) {
        http_write_to_client(rcb, CHUNK_8KB);
    }

    return rcb;
}