#include "mlfb.h"

void mlfb_enqueue(RCB *rcb) {
    if (!rcb) return;

    switch (rcb->chunks_served) {
        case 0: queue_enqueue(top_queue, rcb); break;
        case 1: queue_enqueue(middle_queue, rcb); break;
        default: queue_enqueue(bottom_queue, rcb); break;
    }
}

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
    int len;
    char *buffer = http_create_buffer(send_length);

	len = fread( buffer, 1, send_length, rcb->requested_file );
	if( len < 0 ) {
		perror( "Error while writing to client" );
	} else if( len > 0 ) {
		len = write( rcb->client_connection, buffer, len );
		if( len < 1 ) {
			perror( "Error while writing to client" );
		} else {
			rcb_update_record(rcb, send_length);
		}
	}

	free(buffer);

    /* If the request has been fulfilled, destroy the RCB; otherwise return
    * it to the queue */
    if (rcb_completed(rcb)) {
        rcb_destroy(rcb);
        sem_post(&permission_to_queue);
        return NULL;
    } else {
        return rcb;
    }
}