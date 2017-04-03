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
 * Send a block of a requested file to the client.
 */
RCB* rr_serve(RCB *rcb) {
	int len;
	char *buffer = http_create_buffer(MAX_HTTP_SIZE);

    /* Read up to the maximum size into the write buffer. */
	len = fread( buffer, 1, MAX_HTTP_SIZE, rcb->requested_file );
	if( len < 0 ) {
		perror( "Error while writing to client" );
	} else if( len > 0 ) {
        /* Transmit the buffer contents to the client */
		len = write( rcb->client_connection, buffer, len );
		if( len < 1 ) {
			perror( "Error while writing to client" );
		} else {
            /* Reflect the transmission in the RCB */
			rcb_update_record(rcb, MAX_HTTP_SIZE);
		}
	}

    /* Clear the buffer for the next transfer. */
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

/**
 * Retrieve the RCB from the front of the queue
 */
RCB* rr_dequeue() {
    pthread_mutex_lock(&top_lock);
        RCB *ready_to_serve = queue_dequeue(top_queue);
    pthread_mutex_unlock(&top_lock);

    return ready_to_serve;
}